from fastapi import FastAPI, WebSocket, WebSocketDisconnect, HTTPException, status, Depends, Query
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse, RedirectResponse
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from jose import JWTError, jwt
from datetime import datetime, timedelta, timezone
from pydantic import BaseModel
import os
import asyncio
import json
from typing import Any
import database
import get_info as info
app = FastAPI(title="系统监控API",description="实时监控计算机系统状态")

app.add_middleware(CORSMiddleware,
                   allow_origins=["*"],
                   allow_methods=["*"],
                   allow_headers=["*"],
                   )

#=====================密码 & JWT=====================
SECRET_PASSWORD = "123456"
SECRET_KEY = "your-very-long-random-secret-key-change-this-at-least-32-chars"
ALGORITHM = "HS256"
TOKEN_EXPIRE_MINUTES = 480
#===================================================

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="/login")

class Token(BaseModel):
    access_token: str
    token_type: str

#Database
@app.on_event("startup")
async def startup_event():
    database.init_db()
    asyncio.create_task(monitor_task())
#推送
activate_connections: list[WebSocket] = []
async def broadcast(data: dict):
    if not activate_connections:
        return
    message = json.dumps(data, ensure_ascii=False)
    for connection in activate_connections[:]:
        try:
            await connection.send_text(message)
        except Exception:
            activate_connections.remove(connection)

async def monitor_task():
    while True:
        try:
            data = {
                "platform": info.get_platform_info(),
                "cpu": info.get_cpu_info(),
                "disk": info.get_disk_info(),
                "memory": info.get_memory_info(),
                "network": info.get_network_info(),
                "process": info.get_process_info(),
            }

            with database.get_db() as conn:
                cursor = conn.cursor()
                snapshot_values = (
                    data["platform"].get("platform"),
                    data["platform"].get("system_name"),
                    data["cpu"].get("usage_percent"),
                    data["cpu"].get("physical_core"),
                    data["cpu"].get("logical_core"),
                    data["cpu"].get("time_user"),
                    data["cpu"].get("time_system"),
                    data["cpu"].get("time_idle"),
                    data["memory"]["ram"].get("ram_total"),
                    data["memory"]["ram"].get("ram_used"),
                    data["memory"]["ram"].get("ram_available"),
                    data["memory"]["ram"].get("ram_percent"),
                    data["memory"]["swap"].get("swap_total"),
                    data["memory"]["swap"].get("swap_used"),
                    data["memory"]["swap"].get("swap_free"),
                    data["memory"]["swap"].get("swap_percent"),
                    data["network"]["net_io"].get("bytes_sent"),
                    data["network"]["net_io"].get("bytes_recv"),
                    data["network"]["connections"].get("number"),
                    data["network"]["connections"].get("bool_value")
                )

                cursor.execute("""
                INSERT INTO snapshots (
                    platform, system_name, cpu_usage_percent, cpu_physical_core, cpu_logical_core,
                    cpu_time_user, cpu_time_system, cpu_time_idle,
                    ram_total, ram_used, ram_available, ram_percent,
                    swap_total, swap_used, swap_free, swap_percent,
                    net_bytes_sent, net_bytes_recv, net_connections_number, net_connections_bool_value
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                """, snapshot_values)

                snapshot_id = cursor.lastrowid
                #磁盘数据
                for dev, disk_info in data["disk"].items():
                    if dev != "Error":
                        usage = disk_info.get("usage", {})
                        cursor.execute("""
                        INSERT INTO disk_partitions (
                            snapshot_id, device, mountpoint, fstype,
                            usage_total, usage_used, usage_free, usage_percent
                        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                        """, (
                            snapshot_id, dev, disk_info.get("mountpoint"),
                            disk_info.get("fstype"), usage.get("usage_total"),
                            usage.get("usage_used"), usage.get("usage_free"),
                            usage.get("usage_percent")
                        ))
                #网络数据
                for iface, iface_info in data["network"].get("ifaces", {}).items():
                    addr = iface_info.get("address", {})
                    io = iface_info.get("io", {})
                    stats = iface_info.get("stats", {})
                    ipv4_json = json.dumps(addr.get("ipv4", []))  # 存为 JSON 字符串
                    cursor.execute("""
                    INSERT INTO network_interfaces (
                        snapshot_id, iface, ipv4_addresses,
                        bytes_sent, bytes_recv, isup, speed
                    ) VALUES (?, ?, ?, ?, ?, ?, ?)
                    """, (
                        snapshot_id, iface, ipv4_json,
                        io.get("bytes_sent"), io.get("bytes_recv"),
                        stats.get("isup"), stats.get("speed")
                    ))
                #进程
                processes = list(data["process"].get("processes", {}).values())
                processes.sort(key=lambda p: p.get("cpu_percent", 0), reverse=True)
                for p in processes[:10]:
                    cursor.execute("""
                    INSERT INTO processes (
                        snapshot_id, pid, name, status,
                        cpu_percent, memory_percent, num_threads, create_time
                    ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                    """, (
                        snapshot_id, p.get("pid"), p.get("name"), p.get("status"),
                        p.get("cpu_percent"), p.get("memory_percent"),
                        p.get("num_threads"), p.get("create_time")
                    ))

                conn.commit()

            await broadcast(data)

        except Exception as e:
            print(f"监控任务出错：{e}")

        await asyncio.sleep(30)

#清理历史数据
async def cleanup_task():
    while True:
        try:
            database.cleanup_old_data()
        except Exception as e:
            print(f"清理历史数据失败：{e}")
        await asyncio.sleep(3600)

@app.on_event("startup")
async def startup_event():
    database.init_db()
    asyncio.create_task(monitor_task())
    asyncio.create_task(cleanup_task())

@app.websocket("/monitor/ws")
async def websocket_endpoint(websocket: WebSocket, token: str = Query(...)):
    try:
        jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
    except JWTError:
        await websocket.close(code=status.WS_1008_POLICY_VIOLATION)
        return
    try:
        await websocket.accept()
        activate_connections.append(websocket)
        initial_data = {
            "platform": info.get_platform_info(),
            "cpu": info.get_cpu_info(),
            "disk": info.get_disk_info(),
            "memory": info.get_memory_info(),
            "network": info.get_network_info(),
            "process": info.get_process_info(),
        }
        await websocket.send_json(initial_data)
        await websocket.receive_text()
    except WebSocketDisconnect:
        activate_connections.remove(websocket)
    except Exception as e:
        print(f"WebSocket出错：{e}")
        if websocket in activate_connections:
            activate_connections.remove(websocket)
        await websocket.close(code=status.WS_1011_INTERNAL_ERROR)

#接口
@app.post("/login", response_model=Token)
async def login(form_data: OAuth2PasswordRequestForm = Depends()):
    print(f"收到的用户名: {form_data.username}")
    print(f"收到的密码: '{form_data.password}'") 
    if form_data.password != SECRET_PASSWORD:
        raise HTTPException(status_code=401, detail="密码错误")
    access_token = jwt.encode(
        {"exp": datetime.now(timezone.utc) + timedelta(minutes=TOKEN_EXPIRE_MINUTES)},
        SECRET_KEY,
        algorithm=ALGORITHM
    )
    return Token(access_token=access_token, token_type="bearer")

async def get_current_user(token: str = Depends(oauth2_scheme)):
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
    except JWTError:
        raise HTTPException(status_code=401, detail="无效或过期token")
    return token

@app.get("/monitor/api")
async def root(current_user: Any = Depends(get_current_user)):
    return {
        "platform": info.get_platform_info(),
        "cpu": info.get_cpu_info(),
        "disk": info.get_disk_info(),
        "memory": info.get_memory_info(),
        "network": info.get_network_info(),
        "process": info.get_process_info(),
    }

@app.get("/monitor/api/cpu")
async def cpu(current_user: Any = Depends(get_current_user)):
    return info.get_cpu_info()

@app.get("/monitor/api/disk")
async def disk(current_user: Any = Depends(get_current_user)):
    return info.get_disk_info()

@app.get("/monitor/api/memory")
async def memory(current_user: Any = Depends(get_current_user)):
    return info.get_memory_info()

@app.get("/monitor/api/network")
async def network(current_user: Any = Depends(get_current_user)):
    return info.get_network_info()

@app.get("/monitor/api/proc")
async def process(current_user: Any = Depends(get_current_user)):
    return info.get_process_info()

@app.get("/monitor/api/history")
async def get_history(hours: int = Query(24, description="最近小时数"), current_user: Any = Depends(get_current_user)):
    with database.get_db() as conn:
        cursor = conn.cursor()
        cursor.execute("""
        SELECT * FROM snapshots 
        WHERE timestamp >= datetime('now', ?)
        ORDER BY timestamp DESC
        """, (f"-{hours} hours",))
        snapshots = cursor.fetchall()
        
        # 对于每个 snapshot，查询附属表（简化返回字典列表）
        results = []
        for snap in snapshots:
            snap_dict = dict(zip([col[0] for col in cursor.description], snap))
            snap_id = snap_dict["id"]
            
            # 磁盘
            cursor.execute("SELECT * FROM disk_partitions WHERE snapshot_id = ?", (snap_id,))
            snap_dict["disk"] = [dict(zip([col[0] for col in cursor.description], row)) for row in cursor.fetchall()]
            
            # 网络接口
            cursor.execute("SELECT * FROM network_interfaces WHERE snapshot_id = ?", (snap_id,))
            snap_dict["network_ifaces"] = [dict(zip([col[0] for col in cursor.description], row)) for row in cursor.fetchall()]
            for iface in snap_dict["network_ifaces"]:
                iface["ipv4_addresses"] = json.loads(iface["ipv4_addresses"])  # 解析 JSON
            
            # 进程
            cursor.execute("SELECT * FROM processes WHERE snapshot_id = ?", (snap_id,))
            snap_dict["processes"] = [dict(zip([col[0] for col in cursor.description], row)) for row in cursor.fetchall()]
            
            results.append(snap_dict)
        
        return {"history": results}

@app.get("/monitor")
async def home():
    html_path = os.path.join(os.path.dirname(__file__), "..", "front", "index.html")
    html_path = os.path.normpath(html_path)
    return FileResponse(html_path)

@app.get("/monitor/")
async def serve_spa_root_slash():
    return RedirectResponse(url="/monitor")

@app.get("/monitor/{subpath:path}")
async def serve_spa(subpath: str):
    html_path = os.path.join(os.path.dirname(__file__), "..", "front", "index.html")
    html_path = os.path.normpath(html_path)
    return FileResponse(html_path)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)