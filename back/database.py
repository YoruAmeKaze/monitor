import sqlite3
import os
from contextlib import contextmanager

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DB_PATH = os.path.join(BASE_DIR, "data", "monitor_history.db")  # 数据库文件路径

KEEP_DAYS = 7

def init_db():
    os.makedirs(BASE_DIR, exist_ok=True)
    
    with get_db() as conn:
        cursor = conn.cursor()
        
        #主表
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS snapshots (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT (datetime('now', 'localtime')),
            -- Platform
            platform TEXT,
            system_name TEXT,
            -- CPU
            cpu_usage_percent REAL,
            cpu_physical_core INTEGER,
            cpu_logical_core INTEGER,
            cpu_time_user REAL,
            cpu_time_system REAL,
            cpu_time_idle REAL,
            -- Memory RAM
            ram_total REAL,
            ram_used REAL,
            ram_available REAL,
            ram_percent REAL,
            -- Memory Swap
            swap_total REAL,
            swap_used REAL,
            swap_free REAL,
            swap_percent REAL,
            -- Network Overall
            net_bytes_sent INTEGER,
            net_bytes_recv INTEGER,
            net_connections_number INTEGER,
            net_connections_bool_value BOOLEAN  -- 是否成功获取连接
        )
        """)
        
        # 磁盘
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS disk_partitions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            snapshot_id INTEGER,
            device TEXT,
            mountpoint TEXT,
            fstype TEXT,
            usage_total REAL,
            usage_used REAL,
            usage_free REAL,
            usage_percent REAL,
            FOREIGN KEY (snapshot_id) REFERENCES snapshots(id) ON DELETE CASCADE
        )
        """)
        
        # 网络接口
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS network_interfaces (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            snapshot_id INTEGER,
            iface TEXT,
            ipv4_addresses TEXT,  -- JSON 字符串存储列表，如 '["192.168.1.1/24"]'
            bytes_sent INTEGER,
            bytes_recv INTEGER,
            isup BOOLEAN,
            speed INTEGER,
            FOREIGN KEY (snapshot_id) REFERENCES snapshots(id) ON DELETE CASCADE
        )
        """)
        
        # 进程
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS processes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            snapshot_id INTEGER,
            pid INTEGER,
            name TEXT,
            status TEXT,
            cpu_percent REAL,
            memory_percent REAL,
            num_threads INTEGER,
            create_time INTEGER,  -- Unix 时间戳
            FOREIGN KEY (snapshot_id) REFERENCES snapshots(id) ON DELETE CASCADE
        )
        """)
        
        # 索引
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_snapshot_timestamp ON snapshots(timestamp)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_disk_snapshot_id ON disk_partitions(snapshot_id)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_net_snapshot_id ON network_interfaces(snapshot_id)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_proc_snapshot_id ON processes(snapshot_id)")
        
        conn.commit()

def cleanup_old_data():
    with get_db() as conn:
        cursor = conn.cursor()
        cursor.execute("""
        DELETE FROM snapshots
        WHERE timestamp < datetime('now', ?)
        """, (f'-{KEEP_DAYS} days',))
        conn.commit()

@contextmanager
def get_db():
    conn = sqlite3.connect(DB_PATH)
    try:
        yield conn
    finally:
        conn.close()