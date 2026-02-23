# 🚀 System Monitor Dashboard (实时系统监控仪表盘)

一个轻量级、功能齐全的实时系统监控工具。采用 **FastAPI** 后端与原生 **JS** 前端，通过 **WebSocket** 实现超低延迟的数据传输，并支持 **Docker** 一键部署。

## ✨ 核心功能

* **📈 实时监控**：利用 WebSocket 技术，实现 CPU、内存、网络 IO 及磁盘使用率的秒级实时刷新。
* **🗄️ 历史回溯**：内置 **SQLite** 数据库，自动持久化存储系统快照，支持查看历史运行趋势。
* **⚠️ 危险阈值报警**：
* **警告级别 (Warning)**：指标达到 70% 时，界面显示黄色提醒。
* **危险级别 (Danger)**：指标达到 90% 时，界面显示红色预警，提醒及时处理。


* **🛡️ 安全保障**：集成 **JWT (JSON Web Token)** 认证机制，确保只有通过身份验证的用户才能访问监控数据。
* **🐳 深度 Docker 化**：专门优化的 Dockerfile，支持通过映射 `/proc` 和 `/sys` 目录，在容器内精准监控宿主机真实物理硬件信息。
* **🎨 现代 UI**：基于 Bootstrap 的暗黑模式设计，提供极佳的视觉体验。

---

## 🚀 使用教程

### 方式一：使用 Docker 部署

这是最简单的部署方式，只需一个 `docker-compose.yml` 文件。

1. **创建配置文件**：

```yaml
version: '3.8'

services:
  monitor:
    image: kazever/kazever_monitor:v1.1
    container_name: system_monitor
    network_mode: host
    pid: "host"
    environment:
      - MONITOR_PASSWORD=你的自定义登录密码
      - MONITOR_SECRET_KEY=建议替换为一串随机长字符串
    volumes:
      - ./data:/app/back/data
      - /proc:/host/proc:ro
      - /sys:/host/sys:ro
      - /etc/os-release:/host/etc/os-release:ro
    restart: always

```

2. **启动服务**：

```bash
docker-compose up -d

```

---

### 方式二：直接使用 Python 运行

如果你想在本地直接运行或开发：

1. **安装依赖**：
确保你安装了 `uvicorn[standard]` 以获得 WebSocket 支持。
```bash
pip install -r back/requirements.txt

```


2. **配置环境变量**：
在 `main.py` 中修改密码和密钥的默认值。
3. **启动后端**：
```bash
cd back
python main.py

```


4. **访问界面**：
打开浏览器访问：`http://localhost:8000/monitor`

---

## ⚙️ 环境变量说明

通过环境变量，你可以在不修改代码的情况下快速配置项目：

| 环境变量 | 说明 | 默认值 |
| --- | --- | --- |
| `MONITOR_PASSWORD` | 登录 Web 界面所需的密码 | 默认为`123456` |
| `MONITOR_SECRET_KEY` | JWT 签名的密钥 | 内置默认字符串 |
| `KEEP_DAYS` | 数据库中历史数据保留的天数 | `7` |
| `PROCFS_PATH` | (仅Linux) 宿主机 proc 路径映射 | `/proc` |

---

## 📂 项目结构

```text
├── back/
│   ├── main.py          # FastAPI 主程序及 WebSocket 逻辑
│   ├── get_info.py      # psutil 硬件信息获取模块
│   ├── database.py      # SQLite 初始化与存储逻辑
│   ├── requirements.txt  # Python 依赖列表
│   └── data/            # 存储历史数据
├── front/
│   └── index.html       # 前端 UI
├── Dockerfile           # 镜像构建文件
└── docker-compose.yml   # 容器编排配置

```



---

**如果你觉得这个项目有用，欢迎在 GitHub 上点个 Star！🌟**
