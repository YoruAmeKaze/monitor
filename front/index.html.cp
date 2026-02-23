<!DOCTYPE html>
<html lang="zh-CN">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>系统监控仪表盘</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <style>
        :root {
            --bg-main: #1E1E1E;
            --bg-secondary: #2D2D2D;
            --text-primary: #E0E0E0;
            --text-secondary: #AAAAAA;
            --accent: #00FF99;
            --accent-dark: #00CC77;
            --card-border: #3A3A3A;
            --warning: #FFAA00;
            --danger: #FF4444;
        }

        body {
            background-color: var(--bg-main);
            color: var(--text-primary);
            padding: 20px;
            font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
        }

        .container {
            max-width: 1400px;
        }

        .navbar {
            background-color: var(--bg-secondary) !important;
            border-bottom: 1px solid var(--card-border);
        }

        .navbar-brand,
        .nav-link {
            color: var(--text-primary) !important;
        }

        .nav-link:hover,
        .nav-link.active {
            color: var(--accent) !important;
            background: rgba(0, 255, 153, 0.08);
            border-radius: 6px;
        }

        .card {
            background-color: var(--bg-secondary);
            border: 1px solid var(--card-border);
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
            margin-bottom: 24px;
        }

        .card-header {
            background-color: #0A3D2A !important;
            color: var(--accent) !important;
            font-weight: 600 !important;
            padding: 12px 16px !important;
            border-bottom: 1px solid var(--card-border) !important;
        }

        #processes-section .card-header {
            background-color: #0A3D2A !important;
            color: var(--accent) !important;
        }

        /* 針對動態插入的內容，再加一層保險 */
        .view-section .card-header {
            background-color: #0A3D2A !important;
            color: var(--accent) !important;
        }

        .card-body {
            color: var(--text-primary);
        }

        .text-muted {
            color: var(--text-secondary) !important;
        }

        progress {
            width: 100%;
            height: 10px;
            border-radius: 5px;
            background: #111;
            border: none;
        }

        progress::-webkit-progress-bar {
            background: #111;
            border-radius: 5px;
        }

        progress::-webkit-progress-value {
            background: linear-gradient(90deg, #00FF99, #00CC77);
            border-radius: 5px;
            box-shadow: 0 0 8px rgba(0, 255, 153, 0.5);
        }

        progress::-moz-progress-bar {
            background: linear-gradient(90deg, #00FF99, #00CC77);
            border-radius: 5px;
            box-shadow: 0 0 8px rgba(0, 255, 153, 0.5);
        }

        .btn-primary {
            background: var(--accent);
            border-color: var(--accent-dark);
            color: #0F1F17;
            font-weight: 600;
        }

        .btn-primary:hover {
            background: #00E088;
            border-color: #00B366;
            box-shadow: 0 0 15px rgba(0, 255, 153, 0.4);
        }

        .btn-secondary {
            background: #444;
            border-color: #555;
            color: var(--text-primary);
        }

        .btn-secondary:hover {
            background: #555;
            color: var(--accent);
        }

        .refresh-btn {
            position: fixed;
            bottom: 20px;
            right: 20px;
            z-index: 1000;
            background: var(--accent);
            color: #0F1F17;
            border: none;
            box-shadow: 0 4px 15px rgba(0, 255, 153, 0.35);
        }

        .refresh-btn:hover {
            background: #00E088;
            box-shadow: 0 6px 20px rgba(0, 255, 153, 0.5);
        }

        .alert-badge {
            margin-left: 10px;
            font-size: 0.9em;
        }

        .detail-alert {
            margin-bottom: 20px;
        }

        .badge.bg-warning {
            background-color: var(--warning) !important;
            color: #1E1E1E;
        }

        .badge.bg-danger {
            background-color: var(--danger) !important;
        }

        .alert-warning {
            background: rgba(255, 170, 0, 0.15);
            border-color: rgba(255, 170, 0, 0.4);
            color: #FFDD88;
        }

        .alert-danger {
            background: rgba(255, 68, 68, 0.15);
            border-color: rgba(255, 68, 68, 0.4);
            color: #FFAAAA;
        }

        .table {
            --bs-table-color: #E0E0E0;
            /* 主要內容文字 */
            --bs-table-bg: transparent;
            /* 背景保持卡片顏色 */
            --bs-table-border-color: #3A3A3A;
        }

        .table thead th {
            color: var(--accent);
            /* 表頭保持霓虹綠 */
            background-color: #252525;
            border-bottom: 1px solid #3A3A3A;
        }

        .table td,

        .table tbody td {
            color: #D0D0D0
        }

        .table th {
            border-color: #3A3A3A;
        }

        .table-hover tbody tr:hover {
            background-color: rgba(0, 255, 153, 0.08);
        }

        .spinner-border {
            color: var(--accent) !important;
        }

        a {
            color: var(--accent);
        }

        a:hover {
            color: #00E088;
            text-decoration: underline;
        }

        .view-section {
            display: none;
        }

        .active-view {
            display: block;
        }

        .loading {
            text-align: center;
            padding: 40px;
        }

        .list-group-item {
            background-color: transparent;
            border-color: #3A3A3A;
            color: #D0D0D0;
            /* 列表文字 */
        }

        .list-group-item strong {
            color: #F0F0F0;
        }
    </style>
</head>

<body>
    <div class="container">
        <!-- 导航栏 -->
        <nav class="navbar navbar-expand-lg navbar-dark mb-4 rounded">
            <div class="container-fluid">
                <a class="navbar-brand" href="/monitor">系统监控仪表盘</a>
                <div class="navbar-nav">
                    <a class="nav-link" href="/monitor" data-view="overview">概览</a>
                    <a class="nav-link" href="/monitor/cpu" data-view="cpu">CPU</a>
                    <a class="nav-link" href="/monitor/memory" data-view="memory">内存</a>
                    <a class="nav-link" href="/monitor/disk" data-view="disk">磁盘</a>
                    <a class="nav-link" href="/monitor/network" data-view="network">网络</a>
                    <a class="nav-link" href="/monitor/processes" data-view="processes">进程</a>
                </div>
            </div>
        </nav>

        <div id="last-update" class="text-muted text-center mb-4">最后更新：从未</div>

        <div id="loading" class="loading">
            <div class="spinner-border" role="status"></div>
            <p>正在加载数据...</p>
        </div>

        <!-- 概览页面 -->
        <div id="overview-section" class="view-section">
            <div class="row" id="overview-content"></div>
        </div>

        <!-- CPU 详情 -->
        <div id="cpu-section" class="view-section">
            <a href="/monitor" class="btn btn-secondary mb-3" data-view="overview">← 返回概览</a>
            <div id="cpu-alert" class="detail-alert"></div>
            <div class="card">
                <div class="card-header">CPU 详细信息</div>
                <div class="card-body" id="cpu-detail"></div>
            </div>
        </div>

        <!-- 内存详情 -->
        <div id="memory-section" class="view-section">
            <a href="/monitor" class="btn btn-secondary mb-3" data-view="overview">← 返回概览</a>
            <div id="memory-alert" class="detail-alert"></div>
            <div class="card">
                <div class="card-header">内存详细信息</div>
                <div class="card-body" id="memory-detail"></div>
            </div>
        </div>

        <!-- 磁盘详情 -->
        <div id="disk-section" class="view-section">
            <a href="/monitor" class="btn btn-secondary mb-3" data-view="overview">← 返回概览</a>
            <div id="disk-alert" class="detail-alert"></div>
            <div class="card">
                <div class="card-header">磁盘详细信息</div>
                <div class="card-body" id="disk-detail"></div>
            </div>
        </div>

        <!-- 网络详情 -->
        <div id="network-section" class="view-section">
            <a href="/monitor" class="btn btn-secondary mb-3" data-view="overview">← 返回概览</a>
            <div id="network-alert" class="detail-alert"></div>
            <div class="card">
                <div class="card-header">网络详细信息</div>
                <div class="card-body" id="network-detail"></div>
            </div>
        </div>

        <!-- 进程详情 -->
        <div id="processes-section" class="view-section">
            <a href="/monitor" class="btn btn-secondary mb-3" data-view="overview">← 返回概览</a>
            <div id="processes-alert" class="detail-alert"></div>
            <div class="card">
                <div class="card-header">高CPU占用进程（前10名）</div>
                <div class="card-body" id="processes-detail"></div>
            </div>
        </div>
    </div>

    <!-- 手动刷新按钮 -->
    <button class="btn btn-lg rounded-circle shadow refresh-btn" onclick="fetchData()">
        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" fill="currentColor" viewBox="0 0 16 16">
            <path fill-rule="evenodd" d="M8 3a5 5 0 1 0 4.546 2.914.5.5 0 0 1 .908-.417A6 6 0 1 1 8 2v1z" />
            <path
                d="M8 4.466V.534a.25.25 0 0 1 .41-.192l2.36 1.966c.12.1.12.284 0 .384L8.41 4.658A.25.25 0 0 1 8 4.466z" />
        </svg>
    </button>

    <script>
        // ==================== 配置区 ====================
        const API_URL = '/monitor/api';                // 使用相对路径
        const REFRESH_INTERVAL = 5000;                 // 自动刷新间隔（毫秒）
        // 阈值配置
        const THRESHOLDS = {
            cpu: { usage: 80 },
            memory: { ram: 80, swap: 50 },
            disk: { warning: 70, danger: 90 },
            processes: { cpu: 50 }
        };
        // ==============================================

        let globalData = null;
        let currentView = 'overview';

        // 获取当前视图
        function getViewFromPath() {
            const path = window.location.pathname;
            if (path === '/monitor' || path === '/monitor/') return 'overview';
            const match = path.match(/^\/monitor\/([^\/]+)/);
            if (match) {
                const v = match[1];
                const validViews = ['cpu', 'memory', 'disk', 'network', 'processes'];
                if (validViews.includes(v)) return v;
            }
            return 'overview';
        }

        // 切换视图
        function navigateTo(view) {
            const url = view === 'overview' ? '/monitor' : `/monitor/${view}`;
            history.pushState({ view }, '', url);
            switchView(view);
        }

        function switchView(view) {
            currentView = view;
            document.querySelectorAll('.view-section').forEach(el => el.classList.remove('active-view'));
            document.getElementById(`${view}-section`).classList.add('active-view');
            document.body.offsetHeight;  // 觸發 reflow
            document.querySelectorAll('.nav-link').forEach(link => link.classList.remove('active'));
            const activeLink = document.querySelector(`.nav-link[data-view="${view}"]`);
            if (activeLink) activeLink.classList.add('active');
            renderCurrentView();
        }

        // 验证数据函数（兼容字符串）
        function validatePercent(value) {
            let num = (typeof value === 'string') ? parseFloat(value) : value;
            if (typeof num !== 'number' || isNaN(num) || num < 0 || num > 100) {
                return null;
            }
            return num;
        }

        function validateSize(value) {
            let num = (typeof value === 'string') ? parseFloat(value) : value;
            if (typeof num !== 'number' || isNaN(num) || num < 0) {
                return null;
            }
            return num;
        }

        // 数据获取
        async function fetchData() {
            document.getElementById('loading').style.display = 'block';
            try {
                const response = await fetch(API_URL);
                if (!response.ok) throw new Error('API 请求失败');
                globalData = await response.json();
                document.getElementById('last-update').textContent = `最后更新：${new Date().toLocaleString('zh-CN')}`;
                renderCurrentView();
                document.getElementById('loading').style.display = 'none';
            } catch (err) {
                document.getElementById('loading').innerHTML = `<p class="text-danger">加载失败：${err.message}</p>`;
            }
        }

        let ws = null;
        function connectWebSocket() {
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const wsUrl = `${protocol}//${window.location.host}/monitor/ws`;

            ws = new WebSocket(wsUrl);

            ws.onopen = () => {
                console.log('WebSocket已连接');
                fetchData();
            };


            ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    globalData = data;
                    renderCurrentView();
                    document.getElementById('last-update').textContent =
                        `最后更新：${new Date().toLocaleString('zh-CN')}`;
                } catch (e) {
                    console.error('WebSocket 数据解析失败:', e);
                }
            };

            ws.onclose = () => {
                console.log('WebSocket 断开，5秒后重连');
                setTimeout(connectWebSocket, 5000);
            };

            ws.onerror = (error) => {
                console.error('WebSocket 错误：', error);
            };
        }

        window.addEventListener('load', () => {
            connectWebSocket();
            fetchData();
        });

        // 生成告警提示
        function getAlertBadge(level, message) {
            const bgClass = level === 'danger' ? 'bg-danger' : 'bg-warning';
            return `<span class="badge ${bgClass} alert-badge">${message}</span>`;
        }

        function getDetailAlert(level, message) {
            const alertClass = level === 'danger' ? 'alert-danger' : 'alert-warning';
            return `<div class="alert ${alertClass} detail-alert" role="alert">${message}</div>`;
        }

        // 概览渲染
        function renderOverview(data) {
            const platform = data.platform || {};
            const cpu = data.cpu || {};
            const memory = data.memory || {};
            const disk = data.disk || {};
            const network = data.network || {};
            const processes = Object.values(data.process?.processes || {}).slice(0, 5);

            // 验证数据
            cpu.usage_percent = validatePercent(cpu.usage_percent);
            memory.ram = memory.ram || {};
            memory.ram.ram_percent = validatePercent(memory.ram.ram_percent);
            memory.ram.ram_used = validateSize(memory.ram.ram_used);
            memory.ram.ram_total = validateSize(memory.ram.ram_total);
            memory.swap = memory.swap || {};
            memory.swap.swap_percent = validatePercent(memory.swap.swap_percent);

            for (const device in disk) {
                if (device !== 'Error') {
                    const usage = disk[device].usage || {};
                    usage.usage_percent = validatePercent(usage.usage_percent);
                    usage.usage_used = validateSize(usage.usage_used);
                    usage.usage_total = validateSize(usage.usage_total);
                }
            }

            // 告警检查
            let cpuAlert = cpu.usage_percent !== null && cpu.usage_percent > THRESHOLDS.cpu.usage
                ? getAlertBadge('warning', '高负载') : '';

            let memoryAlert = '';
            if (memory.ram.ram_percent !== null && memory.ram.ram_percent > THRESHOLDS.memory.ram) {
                memoryAlert = getAlertBadge('warning', 'RAM 高');
            } else if (memory.swap.swap_percent !== null && memory.swap.swap_percent > THRESHOLDS.memory.swap) {
                memoryAlert = getAlertBadge('warning', 'Swap 高');
            }

            let diskAlert = '';
            let hasDiskDanger = false, hasDiskWarning = false;
            for (const device in disk) {
                if (device !== 'Error') {
                    const u = disk[device].usage?.usage_percent;
                    if (u !== null) {
                        if (u > THRESHOLDS.disk.danger) hasDiskDanger = true;
                        else if (u > THRESHOLDS.disk.warning) hasDiskWarning = true;
                    }
                }
            }
            if (hasDiskDanger) diskAlert = getAlertBadge('danger', '磁盘危急');
            else if (hasDiskWarning) diskAlert = getAlertBadge('warning', '磁盘警告');

            let processesAlert = '';
            let hasHighProcess = processes.some(p => validatePercent(p.cpu_percent) !== null && p.cpu_percent > THRESHOLDS.processes.cpu);
            if (hasHighProcess) processesAlert = getAlertBadge('warning', '高CPU进程');

            let html = `
                    <div class="col-12 mb-4">
                        <div class="card">
                            <div class="card-header">平台信息</div>
                            <div class="card-body">
                                <p><strong>系统：</strong> ${platform.platform || '未知'}</p>
                                <p><strong>名称：</strong> ${platform.system_name || '未知'}</p>
                            </div>
                        </div>
                    </div>
    
                    <div class="col-md-6 col-lg-4">
                        <div class="card">
                            <div class="card-header">CPU${cpuAlert}</div>
                            <div class="card-body">
                                <p>使用率： <progress value="${cpu.usage_percent ?? 0}" max="100"></progress> <strong>${cpu.usage_percent !== null ? cpu.usage_percent.toFixed(1) + '%' : '错误'}</strong></p>
                                <p>核心：${cpu.physical_core || 0} 物理 / ${cpu.logical_core || 0} 逻辑</p>
                                <a href="/monitor/cpu" class="btn btn-primary btn-sm" data-view="cpu">查看详情 →</a>
                            </div>
                        </div>
                    </div>
    
                    <div class="col-md-6 col-lg-4">
                        <div class="card">
                            <div class="card-header">内存${memoryAlert}</div>
                            <div class="card-body">
                                <p>RAM 使用率： <progress value="${memory.ram.ram_percent ?? 0}" max="100"></progress> <strong>${memory.ram.ram_percent !== null ? memory.ram.ram_percent.toFixed(1) + '%' : '错误'}</strong></p>
                                <p>已用：${memory.ram.ram_used !== null ? memory.ram.ram_used.toFixed(1) + ' GB' : '未知'} / ${memory.ram.ram_total !== null ? memory.ram.ram_total.toFixed(1) + ' GB' : '未知'}</p>
                                <p>Swap 使用率： <strong>${memory.swap.swap_percent !== null ? memory.swap.swap_percent.toFixed(1) + '%' : '错误'}</strong></p>
                                <a href="/monitor/memory" class="btn btn-primary btn-sm" data-view="memory">查看详情 →</a>
                            </div>
                        </div>
                    </div>
    
                    <div class="col-md-6 col-lg-4">
                        <div class="card">
                            <div class="card-header">磁盘（主要分区）${diskAlert}</div>
                            <div class="card-body table-responsive">
                                <table class="table table-sm"><thead><tr><th>设备</th><th>使用率</th><th>已用/总量</th></tr></thead><tbody>`;
            for (const device in disk) {
                if (device === 'Error') continue;
                const info = disk[device];
                const usage = info.usage || {};
                const percent = usage.usage_percent;
                html += `<tr>
                        <td>${info.mountpoint || device}</td>
                        <td><span class="badge bg-${percent !== null ? (percent > THRESHOLDS.disk.danger ? 'danger' : percent > THRESHOLDS.disk.warning ? 'warning' : 'success') : 'secondary'}">${percent !== null ? percent.toFixed(1) + '%' : '错误'}</span></td>
                        <td>${usage.usage_used !== null ? usage.usage_used.toFixed(1) : '未知'} / ${usage.usage_total !== null ? usage.usage_total.toFixed(1) : '未知'} GB</td>
                    </tr>`;
            }
            html += `</tbody></table>
                                <a href="/monitor/disk" class="btn btn-primary btn-sm" data-view="disk">查看详情 →</a>
                            </div>
                        </div>
                    </div>
    
                    <div class="col-md-6 col-lg-4">
                        <div class="card">
                            <div class="card-header">网络</div>
                            <div class="card-body">
                                <p>发送：${network.net_io?.bytes_sent || 0} MB</p>
                                <p>接收：${network.net_io?.bytes_recv || 0} MB</p>
                                <p>活动连接：${network.connections?.number || 0}</p>
                                <a href="/monitor/network" class="btn btn-primary btn-sm" data-view="network">查看详情 →</a>
                            </div>
                        </div>
                    </div>
    
                    <div class="col-12">
                        <div class="card">
                            <div class="card-header">高CPU进程（Top 5）${processesAlert}</div>
                            <div class="card-body table-responsive">
                                <table class="table table-sm table-hover"><thead><tr><th>PID</th><th>名称</th><th>CPU%</th><th>内存%</th></tr></thead><tbody>`;
            processes.forEach(p => {
                const cpuPct = validatePercent(p.cpu_percent);
                const memPct = validatePercent(p.memory_percent);
                html += `<tr><td>${p.pid}</td><td>${p.name || '-'}</td><td>${cpuPct !== null ? cpuPct.toFixed(1) : '错误'}</td><td>${memPct !== null ? memPct.toFixed(2) : '错误'}</td></tr>`;
            });
            html += `</tbody></table>
                                <a href="/monitor/processes" class="btn btn-primary btn-sm" data-view="processes">查看完整列表 →</a>
                            </div>
                        </div>
                    </div>
                `;

            document.getElementById('overview-content').innerHTML = html;
        }

        // CPU 详情
        function renderCpuDetail(cpu) {
            cpu.usage_percent = validatePercent(cpu.usage_percent);
            let alertHtml = '';
            if (cpu.usage_percent !== null && cpu.usage_percent > THRESHOLDS.cpu.usage) {
                alertHtml = getDetailAlert('warning', `CPU 使用率过高 (${cpu.usage_percent.toFixed(1)}%)，超过 ${THRESHOLDS.cpu.usage}% 阈值`);
            }
            document.getElementById('cpu-alert').innerHTML = alertHtml;

            const ul = `<ul class="list-group list-group-flush">
                    <li class="list-group-item"><strong>逻辑核心数：</strong> ${cpu.logical_core || 0}</li>
                    <li class="list-group-item"><strong>物理核心数：</strong> ${cpu.physical_core || 0}</li>
                    <li class="list-group-item"><strong>使用率：</strong> <span class="badge bg-${cpu.usage_percent !== null ? (cpu.usage_percent > 80 ? 'danger' : cpu.usage_percent > 50 ? 'warning' : 'success') : 'secondary'}">${cpu.usage_percent !== null ? cpu.usage_percent.toFixed(1) + '%' : '错误'}</span></li>
                    <li class="list-group-item"><strong>用户时间：</strong> ${cpu.time_user || '未知'} s</li>
                    <li class="list-group-item"><strong>系统时间：</strong> ${cpu.time_system || '未知'} s</li>
                    <li class="list-group-item"><strong>空闲时间：</strong> ${cpu.time_idle || '未知'} s</li>
                </ul>`;
            document.getElementById('cpu-detail').innerHTML = ul;
        }

        // 内存详情
        function renderMemoryDetail(memory) {
            const ram = memory.ram || {};
            const swap = memory.swap || {};
            ram.ram_percent = validatePercent(ram.ram_percent);
            ram.ram_total = validateSize(ram.ram_total);
            ram.ram_used = validateSize(ram.ram_used);
            ram.ram_available = validateSize(ram.ram_available);
            swap.swap_percent = validatePercent(swap.swap_percent);

            let alertHtml = '';
            if (ram.ram_percent !== null && ram.ram_percent > THRESHOLDS.memory.ram) {
                alertHtml += getDetailAlert('warning', `RAM 使用率 ${ram.ram_percent.toFixed(1)}% 超过 ${THRESHOLDS.memory.ram}%`);
            }
            if (swap.swap_percent !== null && swap.swap_percent > THRESHOLDS.memory.swap) {
                alertHtml += getDetailAlert('warning', `Swap 使用率 ${swap.swap_percent.toFixed(1)}% 过高`);
            }
            document.getElementById('memory-alert').innerHTML = alertHtml;

            document.getElementById('memory-detail').innerHTML = `
                    <h6>物理内存 (RAM)</h6>
                    <ul class="list-group list-group-flush mb-4">
                        <li class="list-group-item"><strong>总量：</strong> ${ram.ram_total !== null ? ram.ram_total.toFixed(1) + ' GB' : '未知'}</li>
                        <li class="list-group-item"><strong>已用：</strong> ${ram.ram_used !== null ? ram.ram_used.toFixed(1) + ' GB' : '未知'} (${ram.ram_percent !== null ? ram.ram_percent.toFixed(1) + '%' : '错误'})</li>
                        <li class="list-group-item"><strong>可用：</strong> ${ram.ram_available !== null ? ram.ram_available.toFixed(1) + ' GB' : '未知'}</li>
                    </ul>
                    <h6>交换分区 (Swap)</h6>
                    <ul class="list-group list-group-flush">
                        <li class="list-group-item"><strong>总量：</strong> ${validateSize(swap.swap_total) !== null ? validateSize(swap.swap_total).toFixed(1) + ' GB' : '未知'}</li>
                        <li class="list-group-item"><strong>已用：</strong> ${validateSize(swap.swap_used) !== null ? validateSize(swap.swap_used).toFixed(1) + ' GB' : '未知'} (${swap.swap_percent !== null ? swap.swap_percent.toFixed(1) + '%' : '错误'})</li>
                        <li class="list-group-item"><strong>可用：</strong> ${validateSize(swap.swap_free) !== null ? validateSize(swap.swap_free).toFixed(1) + ' GB' : '未知'}</li>
                    </ul>
                `;
        }

        // 磁盘详情（略微精简，完整版可自行扩展）
        function renderDiskDetail(disk) {
            let alertHtml = '';
            let dangerList = [], warnList = [];
            for (const dev in disk) {
                if (dev !== 'Error') {
                    const p = disk[dev].usage?.usage_percent;
                    if (p !== null) {
                        if (p > THRESHOLDS.disk.danger) dangerList.push(`${disk[dev].mountpoint || dev} (${p.toFixed(1)}%)`);
                        else if (p > THRESHOLDS.disk.warning) warnList.push(`${disk[dev].mountpoint || dev} (${p.toFixed(1)}%)`);
                    }
                }
            }
            if (dangerList.length) alertHtml += getDetailAlert('danger', `危急分区：${dangerList.join('、 ')}`);
            if (warnList.length) alertHtml += getDetailAlert('warning', `警告分区：${warnList.join('、 ')}`);
            document.getElementById('disk-alert').innerHTML = alertHtml;

            let html = '<table class="table table-hover"><thead><tr><th>设备</th><th>挂载点</th><th>文件系统</th><th>总量(GB)</th><th>已用(GB)</th><th>可用(GB)</th><th>使用率</th></tr></thead><tbody>';
            for (const dev in disk) {
                if (dev === 'Error') {
                    html += `<tr><td colspan="7" class="text-danger">错误：${disk[dev].error_info || '未知'}</td></tr>`;
                    continue;
                }
                const info = disk[dev];
                const u = info.usage || {};
                const p = validatePercent(u.usage_percent);
                html += `<tr>
                        <td>${dev}</td>
                        <td>${info.mountpoint || '-'}</td>
                        <td>${info.fstype || '-'}</td>
                        <td>${validateSize(u.usage_total)?.toFixed(1) || '-'}</td>
                        <td>${validateSize(u.usage_used)?.toFixed(1) || '-'}</td>
                        <td>${validateSize(u.usage_free)?.toFixed(1) || '-'}</td>
                        <td><span class="badge bg-${p !== null ? (p > 90 ? 'danger' : p > 70 ? 'warning' : 'success') : 'secondary'}">${p !== null ? p.toFixed(1) + '%' : '错误'}</span></td>
                    </tr>`;
            }
            html += '</tbody></table>';
            document.getElementById('disk-detail').innerHTML = html;
        }

        // 网络详情（保持原样，略微调整文字颜色适应）
        function renderNetworkDetail(network) {
            document.getElementById('network-alert').innerHTML = ''; // 网络暂无阈值告警

            const net_io = network.net_io || {};
            const conn = network.connections || {};
            let ifacesHtml = '<h6 class="mt-4">网络接口详情</h6><table class="table table-sm"><thead><tr><th>接口</th><th>IPv4</th><th>发送(KB)</th><th>接收(KB)</th><th>状态</th><th>速度</th></tr></thead><tbody>';
            for (const iface in network.ifaces || {}) {
                const info = network.ifaces[iface];
                const addr = info.address || {};
                const io = info.io || {};
                const stats = info.stats || {};
                const ipv4 = (addr.ipv4 || []).map(a => a[0] + (a[1] ? '/' + a[1] : '')).join('<br>') || '-';
                ifacesHtml += `<tr>
                        <td>${iface}</td>
                        <td>${ipv4}</td>
                        <td>${io.bytes_sent || 0}</td>
                        <td>${io.bytes_recv || 0}</td>
                        <td>${stats.isup ? '↑ 在线' : '↓ 离线'}</td>
                        <td>${stats.speed || '-'} Mbps</td>
                    </tr>`;
            }
            ifacesHtml += '</tbody></table>';

            const connText = conn.bool_value === false
                ? '<p class="text-danger">获取连接失败（权限不足）</p>'
                : `<p><strong>活动连接数：</strong>${conn.number || 0}</p>`;

            document.getElementById('network-detail').innerHTML = `
                    <h6>总体网络IO</h6>
                    <ul class="list-group list-group-flush mb-4">
                        <li class="list-group-item"><strong>发送总量：</strong> ${net_io.bytes_sent || 0} MB</li>
                        <li class="list-group-item"><strong>接收总量：</strong> ${net_io.bytes_recv || 0} MB</li>
                    </ul>
                    ${connText}
                    ${ifacesHtml}
                `;
        }

        // 进程详情
        function renderProcessesDetail(process) {
            const procs = Object.values(process.processes || {});
            let highProcs = [];
            procs.forEach(p => {
                const cpu = validatePercent(p.cpu_percent);
                if (cpu !== null && cpu > THRESHOLDS.processes.cpu) {
                    highProcs.push(`${p.name || '未知'} (PID ${p.pid}, ${cpu.toFixed(1)}%)`);
                }
            });

            let alertHtml = highProcs.length
                ? getDetailAlert('warning', `高CPU进程（>${THRESHOLDS.processes.cpu}%）：${highProcs.join('、 ')}`)
                : '';
            document.getElementById('processes-alert').innerHTML = alertHtml;

            let html = '<table class="table table-sm table-hover"><thead><tr><th>PID</th><th>名称</th><th>状态</th><th>CPU%</th><th>内存%</th><th>线程</th><th>创建时间</th></tr></thead><tbody>';
            if (procs.length === 0) {
                html += '<tr><td colspan="7" class="text-center">暂无进程数据</td></tr>';
            } else {
                procs.forEach(p => {
                    const cpu = validatePercent(p.cpu_percent);
                    const mem = validatePercent(p.memory_percent);
                    html += `<tr>
                            <td>${p.pid}</td>
                            <td>${p.name || '-'}</td>
                            <td>${p.status || '-'}</td>
                            <td>${cpu !== null ? cpu.toFixed(1) : '错误'}</td>
                            <td>${mem !== null ? mem.toFixed(2) : '错误'}</td>
                            <td>${p.num_threads || 0}</td>
                            <td>${p.create_time ? new Date(p.create_time * 1000).toLocaleString('zh-CN') : '-'}</td>
                        </tr>`;
                });
            }
            html += '</tbody></table>';
            document.getElementById('processes-detail').innerHTML = html;
        }

        // 渲染当前视图
        function renderCurrentView() {
            if (!globalData) return;
            if (currentView === 'overview') renderOverview(globalData);
            else if (currentView === 'cpu') renderCpuDetail(globalData.cpu || {});
            else if (currentView === 'memory') renderMemoryDetail(globalData.memory || {});
            else if (currentView === 'disk') renderDiskDetail(globalData.disk || {});
            else if (currentView === 'network') renderNetworkDetail(globalData.network || {});
            else if (currentView === 'processes') renderProcessesDetail(globalData.process || {});
        }

        // 事件绑定
        document.querySelectorAll('a[data-view]').forEach(link => {
            link.addEventListener('click', e => {
                e.preventDefault();
                navigateTo(link.dataset.view);
            });
        });

        window.addEventListener('popstate', e => {
            const view = e.state?.view || getViewFromPath();
            switchView(view);
        });

        // 初始化
        window.onload = () => {
            currentView = getViewFromPath();
            switchView(currentView);
            history.replaceState({ view: currentView }, '', window.location.href);
            fetchData();
            // setInterval(fetchData, REFRESH_INTERVAL);  // 如需自动刷新请取消注释
        };
    </script>
</body>

</html>