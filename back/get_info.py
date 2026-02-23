import psutil
from typing import Any
import platform


def get_platform_info() -> dict[str, Any]:
    return {
        "platform": platform.platform(),
        "system_name": platform.system(),
    }


def get_cpu_info() -> dict[str, Any]:
    times = psutil.cpu_times()
    return {
        "logical_core": psutil.cpu_count(logical=True),
        "physical_core": psutil.cpu_count(logical=False),
        "usage_percent": psutil.cpu_percent(interval=1),  # float
        "time_user": round(times.user, 2),  # float
        "time_system": round(times.system, 2),
        "time_idle": round(times.idle, 2),
    }


def get_memory_info() -> dict[str, Any]:
    mem = psutil.virtual_memory()
    swap = psutil.swap_memory()

    return {
        "ram": {
            "ram_total": round(mem.total / (1024 ** 3), 2),
            "ram_available": round(mem.available / (1024 ** 3), 2),
            "ram_used": round(mem.used / (1024 ** 3), 2),
            "ram_percent": round(mem.percent, 2),
            "ram_free": round(mem.free / (1024 ** 3), 2),
        },
        "swap": {
            "swap_total": round(swap.total / (1024 ** 3), 2) if swap.total > 0 else 0.0,
            "swap_used": round(swap.used / (1024 ** 3), 2),
            "swap_percent": round(swap.percent, 2) if swap.total > 0 else 0.0,
            "swap_free": round(swap.free / (1024 ** 3), 2),
        }
    }


def get_disk_info() -> dict[str, Any]:
    partitions = psutil.disk_partitions(all=False)
    devices_dict = {}

    for partition in partitions:
        device = partition.device
        devices_dict[device] = {
            "mountpoint": partition.mountpoint,
            "fstype": partition.fstype,
        }

        try:
            usage = psutil.disk_usage(partition.mountpoint)
            devices_dict[device]["usage"] = {
                "usage_total": round(usage.total / (1024 ** 3), 2),
                "usage_used": round(usage.used / (1024 ** 3), 2),
                "usage_free": round(usage.free / (1024 ** 3), 2),
                "usage_percent": round(usage.percent, 1),
            }
        except PermissionError:
            devices_dict[device]["error"] = "PermissionDenied"
        except Exception as e:
            devices_dict[device]["error"] = str(e)

    if not devices_dict:
        devices_dict["Error"] = {"error_info": "No partitions found"}

    return devices_dict


def get_network_info() -> dict[str, Any]:
    net_io_total = psutil.net_io_counters()
    network_dict: dict[str, Any] = {
        "ifaces": {},
        "net_io": {
            "bytes_sent": round(net_io_total.bytes_sent / (1024 ** 2), 2),  # MB, float
            "bytes_recv": round(net_io_total.bytes_recv / (1024 ** 2), 2),
            "packets_sent": net_io_total.packets_sent,  # int
            "packets_recv": net_io_total.packets_recv,
            "errout": net_io_total.errout,
            "errin": net_io_total.errin,
        }
    }

    # 网络接口地址
    for iface, addresses in psutil.net_if_addrs().items():
        network_dict["ifaces"][iface] = {"address": {"ipv4": [], "ipv6": []}}
        for addr in addresses:
            if addr.family.name == "AF_INET":
                network_dict["ifaces"][iface]["address"]["ipv4"].append([addr.address, addr.netmask or ""])
            elif addr.family.name == "AF_INET6":
                network_dict["ifaces"][iface]["address"]["ipv6"].append(addr.address)

    # 每个接口的IO
    for iface, stats in psutil.net_io_counters(pernic=True).items():
        if stats.bytes_sent > 0 or stats.bytes_recv > 0:
            network_dict["ifaces"].setdefault(iface, {})
            network_dict["ifaces"][iface]["io"] = {
                "bytes_sent": round(stats.bytes_sent / 1024, 1),  # KB
                "bytes_recv": round(stats.bytes_recv / 1024, 1),
                "packets_sent": stats.packets_sent,
                "packets_recv": stats.packets_recv,
            }

    # 接口状态
    for iface, stats in psutil.net_if_stats().items():
        network_dict["ifaces"].setdefault(iface, {})
        network_dict["ifaces"][iface]["stats"] = {
            "isup": stats.isup,
            "speed": stats.speed if stats.speed is not None else 0,
            "mtu": stats.mtu,
        }

    # 连接数（简化，只返回数量，避免权限问题时整个字段崩溃）
    try:
        conns = psutil.net_connections(kind="inet")
        network_dict["connections"] = {
            "number": len(conns),
            "bool_value": True,
        }
    except psutil.AccessDenied:
        network_dict["connections"] = {
            "bool_value": False,
            "number": 0
        }

    return network_dict


def get_process_info() -> dict[str, Any]:
    process_dict = {"processes": {}}
    candidates = []

    #预热
    psutil.cpu_percent(interval=0.1)

    for proc in psutil.process_iter(['pid', 'name', 'status', 'memory_percent', 'create_time', 'num_threads']):
        try:
            if proc.info['name'] in ("System Idle Process", "idle", None):
                continue

            # 获取更准确的 per-process cpu percent（需要 interval=None 配合预热）
            cpu_pct = proc.cpu_percent(interval=None) or 0.0

            if cpu_pct > 0:
                candidates.append({
                    "pid": proc.info["pid"],
                    "name": proc.info["name"],
                    "status": proc.info["status"],
                    "cpu_percent": round(cpu_pct, 1),
                    "memory_percent": round(proc.info["memory_percent"], 2),
                    "create_time": proc.info["create_time"],
                    "num_threads": proc.info["num_threads"],
                })
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            continue

    # 排序取前10
    top_processes = sorted(candidates, key=lambda x: x["cpu_percent"], reverse=True)[:10]

    for p in top_processes:
        process_dict["processes"][str(p["pid"])] = p  # pid转str作为key

    return process_dict