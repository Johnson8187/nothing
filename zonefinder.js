export default {
  async fetch(request) {
    const html = `
<!DOCTYPE html>
<html lang="zh-TW">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>ZoneFinder | 北科大圖書館</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <!-- Add Phosphor Icons for better visuals -->
    <script src="https://unpkg.com/@phosphor-icons/web"></script>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Noto+Sans+TC:wght@400;500;700&family=Inter:wght@400;600;800&display=swap');
        
        body { 
            font-family: 'Inter', 'Noto Sans TC', sans-serif; 
            background: radial-gradient(circle at top left, #1e293b, #0f172a);
            background-attachment: fixed;
            color: #f8fafc; 
            overflow-x: hidden; 
            -webkit-tap-highlight-color: transparent;
        }

        /* Enhanced Glassmorphism */
        .glass { 
            background: rgba(30, 41, 59, 0.6); 
            backdrop-filter: blur(16px); 
            -webkit-backdrop-filter: blur(16px);
            border: 1px solid rgba(255,255,255,0.08); 
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);
        }
        
        .glass-strong {
            background: rgba(15, 23, 42, 0.85);
            backdrop-filter: blur(20px);
            border-bottom: 1px solid rgba(255,255,255,0.05);
        }

        .active-tab { 
            background: linear-gradient(135deg, #3b82f6 0%, #2563eb 100%); 
            color: white; 
            box-shadow: 0 4px 12px rgba(59, 130, 246, 0.4);
        }

        .floor-card { 
            transition: all 0.3s cubic-bezier(0.34, 1.56, 0.64, 1); 
            position: relative;
            overflow: hidden;
        }
        
        .floor-card::before {
            content: '';
            position: absolute;
            top: 0; left: 0; right: 0; bottom: 0;
            background: linear-gradient(45deg, transparent, rgba(255,255,255,0.03), transparent);
            transform: translateX(-100%);
            transition: 0.5s;
        }

        .floor-card:hover::before {
            transform: translateX(100%);
        }

        .floor-card:active { transform: scale(0.97); }
        
        .modal-enter { animation: modalSlideUp 0.4s cubic-bezier(0.16, 1, 0.3, 1); }
        @keyframes modalSlideUp { 
            from { opacity: 0; transform: translateY(20px) scale(0.95); } 
            to { opacity: 1; transform: translateY(0) scale(1); } 
        }

        .gradient-text {
            background: linear-gradient(to right, #60a5fa, #a78bfa);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        /* Hide scrollbar but keep functionality */
        .scrollbar-hide::-webkit-scrollbar { display: none; }
        .scrollbar-hide { -ms-overflow-style: none; scrollbar-width: none; }
    </style>
</head>
<body class="min-h-screen pb-28 selection:bg-blue-500/30">

    <!-- Header -->
    <nav class="px-6 py-4 flex justify-between items-center sticky top-0 z-40 glass-strong">
        <div>
            <h1 class="text-2xl font-800 tracking-tighter flex items-center gap-2">
                <i class="ph-fill ph-radar text-blue-500 animate-pulse"></i>
                <span class="gradient-text">ZoneFinder</span>
            </h1>
            <p id="live-clock" class="text-[10px] text-slate-400 font-mono tracking-widest mt-0.5 flex items-center gap-1">
                <span class="w-1.5 h-1.5 rounded-full bg-green-500"></span> LIVE STREAM
            </p>
        </div>
        <button onclick="toggleView()" id="viewBtn" class="bg-slate-800/50 hover:bg-slate-700 text-blue-400 text-xs px-3 py-1.5 rounded-full font-bold border border-blue-400/20 transition-all flex items-center gap-1">
            <i class="ph-bold ph-chart-pie-slice"></i> <span id="viewBtnText">Admin</span>
        </button>
    </nav>

    <!-- Student View -->
    <div id="studentView" class="p-6 max-w-md mx-auto space-y-6">
        
        <!-- Welcome & Quick Action -->
        <div class="flex justify-between items-end">
            <div>
                <p class="text-slate-400 text-xs font-medium mb-1">Welcome back, Student</p>
                <h2 class="text-xl font-bold text-white">Find your spot</h2>
            </div>
            <button onclick="findBestSpot()" class="bg-gradient-to-r from-emerald-500 to-teal-500 text-white text-xs font-bold px-4 py-2 rounded-xl shadow-lg shadow-emerald-500/20 hover:shadow-emerald-500/40 transition-all flex items-center gap-1 active:scale-95">
                <i class="ph-bold ph-magic-wand"></i> Auto-Pick
            </button>
        </div>

        <!-- Filters -->
        <div class="flex space-x-3 overflow-x-auto pb-2 scrollbar-hide">
            <button onclick="filterZones('all')" id="tab-all" class="filter-tab active-tab px-4 py-2 rounded-xl text-xs font-bold whitespace-nowrap transition-all flex items-center gap-1">
                <i class="ph-bold ph-squares-four"></i> All
            </button>
            <button onclick="filterZones('focus')" id="tab-focus" class="filter-tab bg-slate-800/50 border border-white/5 text-slate-400 px-4 py-2 rounded-xl text-xs font-bold whitespace-nowrap transition-all hover:text-white hover:bg-slate-700 flex items-center gap-1">
                <i class="ph-bold ph-book-open"></i> Deep Focus
            </button>
            <button onclick="filterZones('collab')" id="tab-collab" class="filter-tab bg-slate-800/50 border border-white/5 text-slate-400 px-4 py-2 rounded-xl text-xs font-bold whitespace-nowrap transition-all hover:text-white hover:bg-slate-700 flex items-center gap-1">
                <i class="ph-bold ph-users-three"></i> Collab
            </button>
        </div>

        <!-- Cards Container -->
        <div id="zone-container" class="space-y-4">
            <!-- B1 Card -->
            <div class="floor-card glass rounded-3xl p-5 border-l-[6px] border-red-500 group" data-type="collab" onclick="showDetail('B1', '多媒體視聽室', 94, 68, 'Red', ['wifi-high', 'speaker-high', 'armchair'])">
                <div class="flex justify-between items-start mb-3">
                    <div>
                        <div class="flex items-center gap-2 mb-1">
                            <span class="text-[10px] bg-red-500/10 text-red-400 px-2 py-0.5 rounded font-bold uppercase tracking-wider border border-red-500/20">Crowded</span>
                            <i class="ph-fill ph-users text-red-400 text-xs"></i>
                        </div>
                        <h3 class="text-lg font-bold text-white group-hover:text-blue-300 transition-colors">B1F 多媒體區</h3>
                        <p class="text-xs text-slate-400">Multimedia & Discussion</p>
                    </div>
                    <div class="text-right">
                        <p class="text-3xl font-black text-white/10 absolute right-4 top-4 pointer-events-none">B1</p>
                    </div>
                </div>
                <!-- Mini Progress Bar -->
                <div class="flex items-center gap-3 mt-4">
                    <div class="flex-1 h-2 bg-slate-700/50 rounded-full overflow-hidden">
                        <div class="h-full bg-red-500 rounded-full" style="width: 94%"></div>
                    </div>
                    <span class="text-xs font-bold text-red-400 w-8 text-right">94%</span>
                </div>
            </div>

            <!-- 1F Card -->
            <div class="floor-card glass rounded-3xl p-5 border-l-[6px] border-yellow-500 group" data-type="collab" onclick="showDetail('1F', '出入口大廳', 65, 52, 'Yellow', ['coffee', 'info', 'door-open'])">
                <div class="flex justify-between items-start mb-3">
                    <div>
                        <div class="flex items-center gap-2 mb-1">
                            <span class="text-[10px] bg-yellow-500/10 text-yellow-400 px-2 py-0.5 rounded font-bold uppercase tracking-wider border border-yellow-500/20">Moderate</span>
                        </div>
                        <h3 class="text-lg font-bold text-white group-hover:text-blue-300 transition-colors">1F 服務大廳</h3>
                        <p class="text-xs text-slate-400">Service Desk & Lobby</p>
                    </div>
                    <div class="text-right">
                        <p class="text-3xl font-black text-white/10 absolute right-4 top-4 pointer-events-none">1F</p>
                    </div>
                </div>
                <div class="flex items-center gap-3 mt-4">
                    <div class="flex-1 h-2 bg-slate-700/50 rounded-full overflow-hidden">
                        <div class="h-full bg-yellow-500 rounded-full" style="width: 65%"></div>
                    </div>
                    <span class="text-xs font-bold text-yellow-400 w-8 text-right">65%</span>
                </div>
            </div>

            <!-- 2F Card -->
            <div class="floor-card glass rounded-3xl p-5 border-l-[6px] border-emerald-500 group" data-type="focus" onclick="showDetail('2F', '閱覽室', 18, 32, 'Green', ['plug', 'lamp', 'wind'])">
                <div class="flex justify-between items-start mb-3">
                    <div>
                        <div class="flex items-center gap-2 mb-1">
                            <span class="text-[10px] bg-emerald-500/10 text-emerald-400 px-2 py-0.5 rounded font-bold uppercase tracking-wider border border-emerald-500/20">Quiet</span>
                            <i class="ph-fill ph-sparkle text-emerald-400 text-xs"></i>
                        </div>
                        <h3 class="text-lg font-bold text-white group-hover:text-blue-300 transition-colors">2F 靜讀區</h3>
                        <p class="text-xs text-slate-400">Reading Room & Study Carrels</p>
                    </div>
                    <div class="text-right">
                        <p class="text-3xl font-black text-white/10 absolute right-4 top-4 pointer-events-none">2F</p>
                    </div>
                </div>
                <div class="flex items-center gap-3 mt-4">
                    <div class="flex-1 h-2 bg-slate-700/50 rounded-full overflow-hidden">
                        <div class="h-full bg-emerald-500 rounded-full" style="width: 18%"></div>
                    </div>
                    <span class="text-xs font-bold text-emerald-400 w-8 text-right">18%</span>
                </div>
            </div>

            <!-- 3F Card -->
            <div class="floor-card glass rounded-3xl p-5 border-l-[6px] border-emerald-500 group" data-type="focus" onclick="showDetail('3F', '中文圖書區', 35, 30, 'Green', ['books', 'desktop', 'chair'])">
                <div class="flex justify-between items-start mb-3">
                    <div>
                        <div class="flex items-center gap-2 mb-1">
                            <span class="text-[10px] bg-emerald-500/10 text-emerald-400 px-2 py-0.5 rounded font-bold uppercase tracking-wider border border-emerald-500/20">Quiet</span>
                        </div>
                        <h3 class="text-lg font-bold text-white group-hover:text-blue-300 transition-colors">3F 書庫區</h3>
                        <p class="text-xs text-slate-400">Chinese Books & Archives</p>
                    </div>
                    <div class="text-right">
                        <p class="text-3xl font-black text-white/10 absolute right-4 top-4 pointer-events-none">3F</p>
                    </div>
                </div>
                <div class="flex items-center gap-3 mt-4">
                    <div class="flex-1 h-2 bg-slate-700/50 rounded-full overflow-hidden">
                        <div class="h-full bg-emerald-500 rounded-full" style="width: 35%"></div>
                    </div>
                    <span class="text-xs font-bold text-emerald-400 w-8 text-right">35%</span>
                </div>
            </div>
        </div>
    </div>

    <!-- Admin View -->
    <div id="adminView" class="hidden p-6 max-w-md mx-auto space-y-6">
        <div class="glass rounded-3xl p-6 relative overflow-hidden">
            <div class="absolute top-0 right-0 p-4 opacity-10">
                <i class="ph-fill ph-chart-bar text-6xl"></i>
            </div>
            <h3 class="text-lg font-bold mb-4 flex items-center text-white">
                <span class="w-2 h-2 bg-red-500 rounded-full mr-2 animate-pulse"></span>
                Live Heatmap
            </h3>
            <!-- Added container with fixed height to prevent infinite growth -->
            <div class="h-64 relative w-full">
                <canvas id="heatmapChart"></canvas>
            </div>
        </div>
        <div class="grid grid-cols-2 gap-4">
            <div class="glass rounded-3xl p-5 border border-white/5 hover:bg-white/5 transition-colors">
                <div class="flex justify-between items-start mb-2">
                    <i class="ph-duotone ph-clock text-2xl text-blue-400"></i>
                    <span class="text-[10px] bg-blue-500/20 text-blue-300 px-1.5 py-0.5 rounded">EST</span>
                </div>
                <p class="text-xs text-slate-500 uppercase font-bold">Peak Time</p>
                <p class="text-lg font-bold mt-0.5 text-white">14:00 - 16:30</p>
            </div>
            <div class="glass rounded-3xl p-5 border border-white/5 hover:bg-white/5 transition-colors">
                <div class="flex justify-between items-start mb-2">
                    <i class="ph-duotone ph-trend-up text-2xl text-emerald-400"></i>
                    <span class="text-[10px] bg-emerald-500/20 text-emerald-300 px-1.5 py-0.5 rounded">+24%</span>
                </div>
                <p class="text-xs text-slate-500 uppercase font-bold">Efficiency</p>
                <p class="text-lg font-bold mt-0.5 text-white">Optimal</p>
            </div>
        </div>
    </div>

    <!-- Modal -->
    <div id="modal" class="hidden fixed inset-0 z-50 flex items-end sm:items-center justify-center sm:p-6 bg-black/60 backdrop-blur-sm transition-opacity duration-300">
        <div class="modal-enter glass-strong w-full max-w-sm rounded-t-[40px] sm:rounded-[40px] p-8 relative shadow-2xl border-t border-white/10">
            <div class="w-12 h-1.5 bg-slate-700 rounded-full mx-auto mb-6 sm:hidden"></div>
            
            <button onclick="closeModal()" class="absolute top-6 right-6 text-slate-400 hover:text-white bg-slate-800/50 p-2 rounded-full transition-colors">
                <i class="ph-bold ph-x"></i>
            </button>
            
            <div id="modal-content">
                <!-- Content injected by JS -->
            </div>
            
            <button onclick="closeModal()" class="w-full mt-8 bg-blue-600 hover:bg-blue-500 text-white font-bold py-4 rounded-2xl transition-all shadow-lg shadow-blue-500/20 flex justify-center items-center gap-2 active:scale-95">
                <span>Got it</span> <i class="ph-bold ph-check"></i>
            </button>
        </div>
    </div>

    <!-- Bottom Nav -->
    <div class="fixed bottom-6 left-1/2 -translate-x-1/2 w-[90%] max-w-xs glass-strong rounded-[32px] p-2 flex justify-around items-center shadow-2xl border border-white/10 z-40">
        <div id="nav-home" onclick="navAction('home')" class="p-3 bg-blue-500 text-white rounded-2xl transition-all cursor-pointer shadow-lg shadow-blue-500/30">
            <i class="ph-fill ph-house text-xl"></i>
        </div>
        <div id="nav-map" onclick="navAction('map')" class="p-3 text-slate-500 hover:text-slate-300 transition-all cursor-pointer hover:bg-white/5 rounded-2xl">
            <i class="ph-bold ph-map-trifold text-xl"></i>
        </div>
        <div id="nav-profile" onclick="alert('Student Profile System Coming Soon')" class="p-3 text-slate-500 hover:text-slate-300 transition-all cursor-pointer hover:bg-white/5 rounded-2xl">
            <i class="ph-bold ph-user text-xl"></i>
        </div>
    </div>

    <script>
        // Update Clock
        function updateTime() {
            const now = new Date();
            document.getElementById('live-clock').innerHTML = 
                '<span class="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse"></span> ' + 
                now.toLocaleTimeString('en-US', { hour12: false }) + " | NTUT LIB";
        }
        setInterval(updateTime, 1000);
        updateTime();

        // Filter Logic
        function filterZones(type) {
            const cards = document.querySelectorAll('.floor-card');
            const tabs = document.querySelectorAll('.filter-tab');
            
            tabs.forEach(tab => {
                tab.classList.remove('active-tab', 'text-white');
                tab.classList.add('bg-slate-800/50', 'text-slate-400');
            });
            
            const activeTab = document.getElementById('tab-' + type);
            activeTab.classList.remove('bg-slate-800/50', 'text-slate-400');
            activeTab.classList.add('active-tab');
            
            cards.forEach(card => {
                if (type === 'all' || card.getAttribute('data-type') === type) {
                    card.style.display = 'block';
                    card.classList.add('animate-pulse'); // Quick visual feedback
                    setTimeout(() => card.classList.remove('animate-pulse'), 300);
                } else {
                    card.style.display = 'none';
                }
            });
        }

        // Auto Pick Feature
        function findBestSpot() {
            // Logic: Find the card with lowest occupancy
            // For demo, we hardcode 2F as the best
            showDetail('2F', '閱覽室', 18, 32, 'Green', ['plug', 'lamp', 'wind']);
            // Add a little toast notification logic here if desired
        }

        // Show Modal
        function showDetail(floor, name, occ, noise, status, amenities = []) {
            const modal = document.getElementById('modal');
            const content = document.getElementById('modal-content');
            
            let colorClass = 'text-emerald-400';
            let bgClass = 'bg-emerald-500';
            if (status === 'Red') { colorClass = 'text-red-400'; bgClass = 'bg-red-500'; }
            if (status === 'Yellow') { colorClass = 'text-yellow-400'; bgClass = 'bg-yellow-500'; }
            
            // Generate amenities HTML
            const amenitiesHtml = amenities.map(icon => \`
                <div class="flex flex-col items-center gap-1 p-3 bg-slate-800/50 rounded-2xl border border-white/5">
                    <i class="ph-fill ph-\${icon} text-xl text-slate-300"></i>
                </div>
            \`).join('');

            content.innerHTML = \`
                <div class="flex items-center gap-2 mb-2">
                    <span class="text-[10px] font-bold tracking-widest uppercase px-2 py-1 rounded bg-slate-800 text-slate-400 border border-slate-700">Live Status</span>
                </div>
                <h2 class="text-4xl font-black text-white mb-1">\${floor}</h2>
                <h3 class="text-xl text-slate-300 font-medium mb-8">\${name}</h3>
                
                <div class="space-y-6">
                    <!-- Occupancy Meter -->
                    <div>
                        <div class="flex justify-between items-end mb-2">
                            <span class="text-slate-400 text-xs font-bold uppercase">Occupancy</span>
                            <span class="font-bold \${colorClass} text-lg">\${occ}%</span>
                        </div>
                        <div class="w-full bg-slate-800 h-4 rounded-full overflow-hidden border border-white/5">
                            <div class="\${bgClass} h-full rounded-full transition-all duration-1000" style="width: \${occ}%"></div>
                        </div>
                    </div>

                    <!-- Noise Meter -->
                    <div class="grid grid-cols-2 gap-4">
                        <div class="p-4 rounded-2xl bg-slate-800/30 border border-white/5">
                            <span class="text-slate-500 text-xs font-bold uppercase block mb-1">Noise Level</span>
                            <div class="flex items-baseline gap-1">
                                <span class="text-2xl font-bold text-white">\${noise}</span>
                                <span class="text-xs text-slate-500">dB</span>
                            </div>
                        </div>
                        <div class="p-4 rounded-2xl bg-slate-800/30 border border-white/5">
                            <span class="text-slate-500 text-xs font-bold uppercase block mb-1">Environment</span>
                            <div class="flex items-baseline gap-1">
                                <span class="text-lg font-bold \${colorClass}">\${status === 'Green' ? 'Excellent' : (status === 'Yellow' ? 'Fair' : 'Busy')}</span>
                            </div>
                        </div>
                    </div>

                    <!-- Amenities -->
                    <div>
                        <span class="text-slate-500 text-xs font-bold uppercase block mb-3">Amenities Available</span>
                        <div class="grid grid-cols-4 gap-3">
                            \${amenitiesHtml}
                            <div class="flex flex-col items-center gap-1 p-3 bg-slate-800/50 rounded-2xl border border-white/5 opacity-50">
                                <i class="ph-bold ph-dots-three text-xl text-slate-300"></i>
                            </div>
                        </div>
                    </div>
                </div>
            \`;
            modal.classList.remove('hidden');
        }

        function closeModal() {
            document.getElementById('modal').classList.add('hidden');
        }

        // Navigation Action
        function navAction(tab) {
            const homeBtn = document.getElementById('nav-home');
            const mapBtn = document.getElementById('nav-map');
            const studentView = document.getElementById('studentView');
            
            // Reset styles
            const inactiveClass = "p-3 text-slate-500 hover:text-slate-300 transition-all cursor-pointer hover:bg-white/5 rounded-2xl";
            const activeClass = "p-3 bg-blue-500 text-white rounded-2xl transition-all cursor-pointer shadow-lg shadow-blue-500/30";
            
            // Reset icons to outline style
            homeBtn.className = inactiveClass;
            homeBtn.querySelector('i').classList.replace('ph-fill', 'ph-bold');
            mapBtn.className = inactiveClass;
            mapBtn.querySelector('i').classList.replace('ph-fill', 'ph-bold');

            // Ensure we are in student view if navigating bottom tabs
            if (studentView.classList.contains('hidden')) {
                toggleView();
            }

            if (tab === 'home') {
                homeBtn.className = activeClass;
                homeBtn.querySelector('i').classList.replace('ph-bold', 'ph-fill');
                window.scrollTo({ top: 0, behavior: 'smooth' });
            } else if (tab === 'map') {
                mapBtn.className = activeClass;
                mapBtn.querySelector('i').classList.replace('ph-bold', 'ph-fill');
                // Scroll to the zone list
                document.getElementById('zone-container').scrollIntoView({ behavior: 'smooth', block: 'start' });
            }
        }

        // Toggle Views
        function toggleView() {
            const student = document.getElementById('studentView');
            const admin = document.getElementById('adminView');
            const btnText = document.getElementById('viewBtnText');
            
            if (student.classList.contains('hidden')) {
                student.classList.remove('hidden');
                admin.classList.add('hidden');
                btnText.innerText = 'Admin';
            } else {
                student.classList.add('hidden');
                admin.classList.remove('hidden');
                btnText.innerText = 'Map';
                setTimeout(initChart, 100);
            }
        }

        let myChart = null;
        function initChart() {
            const ctx = document.getElementById('heatmapChart').getContext('2d');
            if (myChart) myChart.destroy();
            
            // Create gradient
            const gradientRed = ctx.createLinearGradient(0, 0, 0, 400);
            gradientRed.addColorStop(0, 'rgba(239, 68, 68, 0.8)');
            gradientRed.addColorStop(1, 'rgba(239, 68, 68, 0.2)');

            const gradientGreen = ctx.createLinearGradient(0, 0, 0, 400);
            gradientGreen.addColorStop(0, 'rgba(34, 197, 94, 0.8)');
            gradientGreen.addColorStop(1, 'rgba(34, 197, 94, 0.2)');

            myChart = new Chart(ctx, {
                type: 'bar',
                data: {
                    labels: ['B1', '1F', '2F', '3F'],
                    datasets: [{
                        label: 'Occupancy',
                        data: [94, 65, 18, 35],
                        backgroundColor: [gradientRed, '#f59e0b', gradientGreen, gradientGreen],
                        borderRadius: 8,
                        borderWidth: 0,
                        barThickness: 40
                    }]
                },
                options: { 
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: { legend: { display: false } },
                    scales: { 
                        y: { 
                            beginAtZero: true, 
                            max: 100, 
                            grid: { color: 'rgba(255,255,255,0.05)', drawBorder: false },
                            ticks: { color: '#94a3b8', font: { family: 'Inter' } }
                        },
                        x: { 
                            grid: { display: false },
                            ticks: { color: '#fff', font: { weight: 'bold' } }
                        }
                    },
                    animation: {
                        duration: 1500,
                        easing: 'easeOutQuart'
                    }
                }
            });
        }
    </script>
</body>
</html>
    `;
    return new Response(html, {
      headers: { "content-type": "text/html;charset=UTF-8" },
    });
  },
};
