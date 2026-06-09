import pygame
import websocket
import json
import math
import random
import threading
import queue
import sys
import time

# Initialize pygame
pygame.init()
pygame.font.init()

# Game Constants
MAP_SIZE = 5000
SCREEN_WIDTH = 1200
SCREEN_HEIGHT = 800
GRID_SIZE = 60
BASE_SPEED = 4
BOOST_SPEED = 7
SPACING = 12
SHRINK_RATE = 10
MIN_BOOST_LENGTH = 10

# Color Palette (Dark Theme with Vibrant Neon Accents)
BG_COLOR = (14, 14, 18)
GRID_COLOR = (24, 24, 32)
BORDER_COLOR = (255, 50, 80)
TEXT_COLOR = (240, 240, 248)
UI_BG_COLOR = (22, 22, 30, 210)  # RGBA
NEON_CYAN = (0, 255, 204)
NEON_RED = (255, 60, 100)
NEON_GREEN = (50, 255, 100)
NEON_BLUE = (0, 150, 255)
NEON_PURPLE = (180, 50, 255)
NEON_YELLOW = (255, 220, 50)

FOOD_COLORS = [
    (255, 100, 100), (100, 255, 100), (100, 100, 255),
    (255, 255, 100), (255, 100, 255), (100, 255, 255),
    (255, 150, 50), (150, 50, 255), (50, 255, 150)
]

# Set up screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.RESIZABLE)
pygame.display.set_caption("Cyber Snake.io - 英雄賽博競技場")
clock = pygame.time.Clock()

# Helper: Load Chinese-compatible System Fonts
def get_font(size, bold=False):
    font_names = ["microsoftjhenghei", "微軟正黑體", "dengxian", "malgungothic", "arial"]
    return pygame.font.SysFont(font_names, size, bold=bold)

# Fonts
font_title = get_font(50, bold=True)
font_large = get_font(28, bold=True)
font_medium = get_font(18, bold=True)
font_small = get_font(15)
font_bold_small = get_font(15, bold=True)
font_micro = get_font(12)

# Generate Parallax Stars (Distant layer)
PARALLAX_STARS = []
for _ in range(120):
    PARALLAX_STARS.append({
        "x": random.randint(0, MAP_SIZE),
        "y": random.randint(0, MAP_SIZE),
        "size": random.randint(1, 3),
        "color": random.choice([(100, 100, 120), (150, 150, 170), (80, 120, 140)])
    })

class NetworkClient:
    def __init__(self):
        self.ws = None
        self.send_queue = queue.Queue()
        self.receive_queue = queue.Queue()
        self.connected = False
        self.running = False
        self.error_msg = ""
        self.client_id = None

    def connect(self, host):
        self.running = True
        self.error_msg = ""
        url = f"ws://{host}" if not host.startswith("ws://") and not host.startswith("wss://") else host
        threading.Thread(target=self._run_socket, args=(url,), daemon=True).start()

    def _run_socket(self, url):
        websocket.enableTrace(False)
        try:
            self.ws = websocket.WebSocketApp(
                url,
                on_open=self.on_open,
                on_message=self.on_message,
                on_error=self.on_error,
                on_close=self.on_close
            )
            self.ws.run_forever()
        except Exception as e:
            self.error_msg = str(e)
            self.connected = False

    def on_open(self, ws):
        self.connected = True
        threading.Thread(target=self._send_loop, daemon=True).start()

    def on_message(self, ws, message):
        try:
            data = json.loads(message)
            self.receive_queue.put(data)
        except Exception:
            pass

    def on_error(self, ws, error):
        self.error_msg = "連線發生錯誤"

    def on_close(self, ws, close_status_code, close_msg):
        self.connected = False
        self.running = False

    def _send_loop(self):
        while self.running and self.connected:
            try:
                msg = self.send_queue.get(timeout=0.1)
                if self.ws and self.connected:
                    self.ws.send(json.dumps(msg))
            except queue.Empty:
                continue
            except Exception:
                break

    def send(self, msg):
        if self.connected:
            self.send_queue.put(msg)

    def close(self):
        self.running = False
        if self.ws:
            self.ws.close()

class Particle:
    def __init__(self, x, y, color, particle_type="normal"):
        self.x = x
        self.y = y
        self.color = color
        self.type = particle_type
        
        angle = random.uniform(0, 2 * math.pi)
        
        if self.type == "shockwave":
            self.vx = 0
            self.vy = 0
            self.life = 15
            self.max_life = 15
            self.size = 5
        elif self.type == "spark":
            speed = random.uniform(0.5, 2)
            self.vx = math.cos(angle) * speed
            self.vy = math.sin(angle) * speed
            self.life = random.randint(10, 25)
            self.max_life = self.life
            self.size = random.randint(2, 4)
        else: # normal explosion / flame
            speed = random.uniform(1.5, 5)
            self.vx = math.cos(angle) * speed
            self.vy = math.sin(angle) * speed
            self.life = random.randint(20, 40)
            self.max_life = self.life
            self.size = random.randint(3, 8)

    def update(self):
        if self.type == "shockwave":
            self.size += 3.5
        else:
            self.x += self.vx
            self.y += self.vy
            self.vx *= 0.95
            self.vy *= 0.95
        self.life -= 1

    def draw(self, surface, camera_x, camera_y):
        alpha = int((self.life / self.max_life) * 255)
        pos_x = int(self.x - camera_x)
        pos_y = int(self.y - camera_y)
        
        if -100 <= pos_x <= SCREEN_WIDTH + 100 and -100 <= pos_y <= SCREEN_HEIGHT + 100:
            if self.type == "shockwave":
                s = pygame.Surface((self.size * 2, self.size * 2), pygame.SRCALPHA)
                pygame.draw.circle(s, (*self.color, alpha), (self.size, self.size), self.size, width=2)
                surface.blit(s, (pos_x - self.size, pos_y - self.size))
            else:
                s = pygame.Surface((self.size * 2, self.size * 2), pygame.SRCALPHA)
                pygame.draw.circle(s, (*self.color, alpha), (self.size, self.size), self.size)
                surface.blit(s, (pos_x - self.size, pos_y - self.size))

def update_body_segments(path_history, length, spacing):
    segments = []
    if not path_history:
        return segments
    
    segments.append(path_history[0])
    last_point = path_history[0]
    accumulated_dist = 0
    
    for p in path_history[1:]:
        dx = p[0] - last_point[0]
        dy = p[1] - last_point[1]
        d = math.hypot(dx, dy)
        accumulated_dist += d
        last_point = p
        
        while accumulated_dist >= spacing:
            ratio = (spacing - (accumulated_dist - d)) / d if d > 0 else 0
            ratio = max(0.0, min(1.0, ratio))
            x = last_point[0] - dx * (1 - ratio)
            y = last_point[1] - dy * (1 - ratio)
            segments.append((x, y))
            accumulated_dist -= spacing
            
            if len(segments) >= length:
                return segments
                
    while len(segments) < length:
        segments.append(path_history[-1])
    return segments

def draw_shield(surface, center, color, timer_ticks):
    radius = 28
    glow_surf = pygame.Surface((radius * 2 + 10, radius * 2 + 10), pygame.SRCALPHA)
    pulse = int(math.sin(timer_ticks * 0.1) * 3)
    r_outer = radius + pulse
    
    pygame.draw.circle(glow_surf, (*color, 40), (r_outer + 5, r_outer + 5), r_outer)
    pygame.draw.circle(glow_surf, (*color, 120), (r_outer + 5, r_outer + 5), r_outer, width=3)
    surface.blit(glow_surf, (center[0] - r_outer - 5, center[1] - r_outer - 5))

def render_lobby(name, server_addr, active_field, selected_class, error_msg, loading=False, mouse_pos=(0, 0)):
    screen.fill(BG_COLOR)
    
    # Draw Grid
    for x in range(0, SCREEN_WIDTH, GRID_SIZE):
        pygame.draw.line(screen, GRID_COLOR, (x, 0), (x, SCREEN_HEIGHT))
    for y in range(0, SCREEN_HEIGHT, GRID_SIZE):
        pygame.draw.line(screen, GRID_COLOR, (0, y), (SCREEN_WIDTH, y))

    # Title
    title_text = font_title.render("賽博英雄貪吃蛇 (Cyber Arena)", True, NEON_CYAN)
    screen.blit(title_text, (SCREEN_WIDTH // 2 - title_text.get_width() // 2, 40))
    
    subtitle = font_medium.render("選擇你的貪吃蛇英雄職業，加入多人競技場！", True, (150, 150, 160))
    screen.blit(subtitle, (SCREEN_WIDTH // 2 - subtitle.get_width() // 2, 98))

    # --- HERO CLASS CARDS SELECTOR ---
    card_w, card_h = 240, 200
    card_y = 140
    
    classes_info = [
        {
            "id": "hunter",
            "name": "🔴 掠奪者 (Hunter)",
            "desc1": "• 被動: 進食範圍 + 25%",
            "desc2": "• 被動: 吞食死體獲分加倍",
            "active": "【主動: 幽靈突襲】(CD 12秒)",
            "active_desc": "進入2秒半透明幽靈化，無敵且穿身",
            "color": NEON_RED,
            "x": SCREEN_WIDTH // 2 - 380
        },
        {
            "id": "guardian",
            "name": "🟢 守衛者 (Guardian)",
            "desc1": "• 被動: 初始長度 25 節",
            "desc2": "• 被動: 加速長度消耗減半",
            "active": "【主動: 鐵壁充能】(CD 15秒)",
            "active_desc": "展開4秒力場盾，免疫碰撞",
            "color": NEON_GREEN,
            "x": SCREEN_WIDTH // 2 - 120
        },
        {
            "id": "speedster",
            "name": "🔵 風暴使者 (Speedster)",
            "desc1": "• 被動: 跑速增加 20%",
            "desc2": "• 被動: 轉向更加靈敏",
            "active": "【主動: 磁場超載】(CD 10秒)",
            "active_desc": "超載吸食半徑350px內的所有食物",
            "color": NEON_BLUE,
            "x": SCREEN_WIDTH // 2 + 140
        }
    ]

    for card in classes_info:
        card_x = card["x"]
        is_selected = selected_class == card["id"]
        is_hovered = (card_x <= mouse_pos[0] <= card_x + card_w and card_y <= mouse_pos[1] <= card_y + card_h)
        bg_alpha = 245 if (is_selected or is_hovered) else 165
        border_width = 3 if (is_selected or is_hovered) else 1
        border_color = card["color"] if (is_selected or is_hovered) else (60, 60, 80)
        
        # Card Background
        card_surface = pygame.Surface((card_w, card_h), pygame.SRCALPHA)
        pygame.draw.rect(card_surface, (28, 28, 38, bg_alpha), (0, 0, card_w, card_h), border_radius=12)
        pygame.draw.rect(card_surface, border_color, (0, 0, card_w, card_h), width=border_width, border_radius=12)
        screen.blit(card_surface, (card_x, card_y))
        
        # Class Name
        c_title = font_bold_small.render(card["name"], True, card["color"])
        screen.blit(c_title, (card_x + 15, card_y + 15))
        
        # Passive Description
        d1 = font_small.render(card["desc1"], True, TEXT_COLOR)
        d2 = font_small.render(card["desc2"], True, TEXT_COLOR)
        screen.blit(d1, (card_x + 15, card_y + 45))
        screen.blit(d2, (card_x + 15, card_y + 68))
        
        # Active Skill Name
        act = font_bold_small.render(card["active"], True, NEON_YELLOW)
        screen.blit(act, (card_x + 15, card_y + 105))
        
        # Active Description
        act_d = font_micro.render(card["active_desc"], True, (160, 160, 175))
        screen.blit(act_d, (card_x + 15, card_y + 130))
        
        # Selected Indicator
        if is_selected:
            sel_txt = font_bold_small.render("[ 已選擇 ]", True, card["color"])
            screen.blit(sel_txt, (card_x + card_w - sel_txt.get_width() - 15, card_y + card_h - 25))

    # --- INPUT BOXES ---
    box_w, box_h = 500, 240
    box_x = SCREEN_WIDTH // 2 - box_w // 2
    box_y = 365
    
    box_surface = pygame.Surface((box_w, box_h), pygame.SRCALPHA)
    pygame.draw.rect(box_surface, UI_BG_COLOR, (0, 0, box_w, box_h), border_radius=15)
    pygame.draw.rect(box_surface, (0, 255, 204, 80), (0, 0, box_w, box_h), width=2, border_radius=15)
    screen.blit(box_surface, (box_x, box_y))

    # 1. Player Name
    name_lbl = font_bold_small.render("玩家名稱", True, NEON_CYAN)
    screen.blit(name_lbl, (box_x + 50, box_y + 20))
    
    name_bg = (40, 40, 55) if active_field == "name" else (30, 30, 40)
    name_border = NEON_CYAN if active_field == "name" else (70, 70, 90)
    pygame.draw.rect(screen, name_bg, (box_x + 50, box_y + 40, 400, 40), border_radius=8)
    pygame.draw.rect(screen, name_border, (box_x + 50, box_y + 40, 400, 40), width=2, border_radius=8)
    
    name_txt = font_medium.render(name, True, TEXT_COLOR)
    screen.blit(name_txt, (box_x + 65, box_y + 48))

    # 2. Server Address
    srv_lbl = font_bold_small.render("伺服器位址 (IP:PORT 或 網址)", True, NEON_CYAN)
    screen.blit(srv_lbl, (box_x + 50, box_y + 90))
    
    srv_bg = (40, 40, 55) if active_field == "server" else (30, 30, 40)
    srv_border = NEON_CYAN if active_field == "server" else (70, 70, 90)
    pygame.draw.rect(screen, srv_bg, (box_x + 50, box_y + 110, 400, 40), border_radius=8)
    pygame.draw.rect(screen, srv_border, (box_x + 50, box_y + 110, 400, 40), width=2, border_radius=8)
    
    srv_txt = font_medium.render(server_addr, True, TEXT_COLOR)
    screen.blit(srv_txt, (box_x + 65, box_y + 118))

    # Quick Select Options
    quick_lbl = font_small.render("點擊下方按鈕快速填入伺服器位址：", True, (150, 150, 160))
    screen.blit(quick_lbl, (box_x + 50, box_y + 160))
    
    btn_local = font_bold_small.render("[ 本地伺服器 ]", True, (100, 200, 255))
    screen.blit(btn_local, (box_x + 50, box_y + 185))
    
    btn_cf = font_bold_small.render("[ 雲端伺服器 (Workers) ]", True, NEON_CYAN)
    screen.blit(btn_cf, (box_x + 150, box_y + 185))

    # Play & Exit Buttons
    play_btn_y = box_y + 215
    
    play_hover = (box_x + 60 <= mouse_pos[0] <= box_x + 230 and play_btn_y <= mouse_pos[1] <= play_btn_y + 45)
    exit_hover = (box_x + 270 <= mouse_pos[0] <= box_x + 440 and play_btn_y <= mouse_pos[1] <= play_btn_y + 45)
    
    # 進入遊戲
    play_color = (0, 200, 150) if not loading else (100, 100, 100)
    if play_hover and not loading:
        play_color = (0, 240, 180)
    pygame.draw.rect(screen, play_color, (box_x + 60, play_btn_y, 170, 45), border_radius=10)
    if play_hover and not loading:
        pygame.draw.rect(screen, NEON_CYAN, (box_x + 60, play_btn_y, 170, 45), width=2, border_radius=10)
    play_lbl = "連線中..." if loading else "進入遊戲"
    play_txt = font_large.render(play_lbl, True, TEXT_COLOR)
    screen.blit(play_txt, (box_x + 60 + 85 - play_txt.get_width() // 2, play_btn_y + 22 - play_txt.get_height() // 2))
    
    # 離開遊戲
    exit_color = (255, 60, 100)
    if exit_hover:
        exit_color = (255, 100, 130)
    pygame.draw.rect(screen, exit_color, (box_x + 270, play_btn_y, 170, 45), border_radius=10)
    if exit_hover:
        pygame.draw.rect(screen, (255, 200, 220), (box_x + 270, play_btn_y, 170, 45), width=2, border_radius=10)
    exit_txt = font_large.render("離開遊戲", True, TEXT_COLOR)
    screen.blit(exit_txt, (box_x + 270 + 85 - exit_txt.get_width() // 2, play_btn_y + 22 - exit_txt.get_height() // 2))

    # Status / Error MSG
    if error_msg:
        err_txt = font_small.render(error_msg, True, (255, 100, 100))
        screen.blit(err_txt, (SCREEN_WIDTH // 2 - err_txt.get_width() // 2, SCREEN_HEIGHT - 35))

    pygame.display.flip()

# Separate screen logic for Lobby
def run_lobby_screen(player_name, server_address, selected_class):
    active_field = "name"
    error_msg = ""
    loading = False
    net_client = None

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return "quit"
            elif event.type == pygame.VIDEORESIZE:
                global SCREEN_WIDTH, SCREEN_HEIGHT, screen
                SCREEN_WIDTH, SCREEN_HEIGHT = event.size
                screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.RESIZABLE)
                
            elif event.type == pygame.MOUSEBUTTONDOWN:
                mx, my = event.pos
                
                card_y = 140
                card_w, card_h = 240, 200
                c1_x = SCREEN_WIDTH // 2 - 380
                if c1_x <= mx <= c1_x + card_w and card_y <= my <= card_y + card_h:
                    selected_class = "hunter"
                c2_x = SCREEN_WIDTH // 2 - 120
                if c2_x <= mx <= c2_x + card_w and card_y <= my <= card_y + card_h:
                    selected_class = "guardian"
                c3_x = SCREEN_WIDTH // 2 + 140
                if c3_x <= mx <= c3_x + card_w and card_y <= my <= card_y + card_h:
                    selected_class = "speedster"

                box_x = SCREEN_WIDTH // 2 - 250
                box_y = 365
                if box_x + 50 <= mx <= box_x + 450 and box_y + 40 <= my <= box_y + 80:
                    active_field = "name"
                elif box_x + 50 <= mx <= box_x + 450 and box_y + 110 <= my <= box_y + 150:
                    active_field = "server"
                elif box_x + 50 <= mx <= box_x + 130 and box_y + 185 <= my <= box_y + 205:
                    server_address = "localhost:8765"
                    active_field = "server"
                elif box_x + 150 <= mx <= box_x + 350 and box_y + 185 <= my <= box_y + 205:
                    server_address = "wss://ntut-ee-snake-server.johnsontw102060.workers.dev/ws"
                    active_field = "server"
                elif box_x + 60 <= mx <= box_x + 230 and box_y + 215 <= my <= box_y + 260 and not loading:
                    loading = True
                    error_msg = "正在連線至伺服器..."
                    net_client = NetworkClient()
                    net_client.connect(server_address)
                elif box_x + 270 <= mx <= box_x + 440 and box_y + 215 <= my <= box_y + 260:
                    return "quit"
                    
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_TAB:
                    active_field = "server" if active_field == "name" else "name"
                elif event.key == pygame.K_RETURN and not loading:
                    loading = True
                    error_msg = "正在連線至伺服器..."
                    net_client = NetworkClient()
                    net_client.connect(server_address)
                elif event.key == pygame.K_BACKSPACE:
                    if active_field == "name":
                        player_name = player_name[:-1]
                    else:
                        server_address = server_address[:-1]
                else:
                    if event.unicode.isprintable():
                        if active_field == "name" and len(player_name) < 15:
                            player_name += event.unicode
                        elif active_field == "server" and len(server_address) < 80:
                            server_address += event.unicode

        if loading and net_client:
            if net_client.connected:
                return (player_name, server_address, selected_class, net_client)
            elif net_client.error_msg:
                error_msg = net_client.error_msg
                loading = False
                net_client.close()
                net_client = None

        mx, my = pygame.mouse.get_pos()
        render_lobby(player_name, server_address, active_field, selected_class, error_msg, loading, mouse_pos=(mx, my))
        clock.tick(30)

# Separate screen logic for Gameplay Loop
def run_gameplay(player_name, server_address, p_class, net_client):
    # Player's local class-specific configurations
    speed_factor = 1.2 if p_class == "speedster" else 1.0
    turn_factor = 1.6 if p_class == "speedster" else 1.0
    
    length = 25 if p_class == "guardian" else 15
    snake_x = random.randint(500, MAP_SIZE - 500)
    snake_y = random.randint(500, MAP_SIZE - 500)
    snake_angle = random.uniform(0, 2 * math.pi)
    
    path_history = [(snake_x, snake_y)]
    for i in range(1, length * SPACING):
        path_history.append((snake_x - math.cos(snake_angle) * i, snake_y - math.sin(snake_angle) * i))
        
    local_body = update_body_segments(path_history, length, SPACING)
    
    class_colors = {
        "hunter": NEON_RED,
        "guardian": NEON_GREEN,
        "speedster": NEON_BLUE
    }
    color = class_colors[p_class]
    score = length
    is_dead = False
    error_msg = ""
    
    skill_active = False
    skill_active_timer = 0
    skill_cooldown_timer = 0
    
    cooldowns = {"hunter": 12.0, "guardian": 15.0, "speedster": 10.0}
    durations = {"hunter": 2.0, "guardian": 4.0, "speedster": 5.0}
    
    powerup_shield_timer = 0
    powerup_magnet_timer = 0
    powerup_speed_timer = 0
    
    other_snakes = {}
    food_dict = {}
    powerup_dict = {}
    particles = []
    active_toasts = []
    
    net_client.send({
        "type": "join",
        "name": player_name,
        "color": color,
        "class": p_class,
        "body": local_body
    })

    last_update_time = 0
    update_interval = 0.03
    boost_frame_counter = 0
    tick_count = 0

    running = True
    while running:
        dt = clock.tick(60) / 1000.0
        tick_count += 1
        
        if skill_cooldown_timer > 0:
            skill_cooldown_timer -= dt
        if skill_active_timer > 0:
            skill_active_timer -= dt
            if skill_active_timer <= 0:
                skill_active = False
                
        for toast in list(active_toasts):
            toast["life"] -= dt
            if toast["life"] <= 0:
                active_toasts.remove(toast)
                
        if powerup_shield_timer > 0:
            powerup_shield_timer -= dt
        if powerup_magnet_timer > 0:
            powerup_magnet_timer -= dt
        if powerup_speed_timer > 0:
            powerup_speed_timer -= dt

        while not net_client.receive_queue.empty():
            msg = net_client.receive_queue.get()
            msg_type = msg.get("type")
            
            if msg_type == "init":
                net_client.client_id = msg.get("id")
                foods = msg.get("foods", [])
                for f in foods:
                    food_dict[f["id"]] = f
                pwrs = msg.get("powerups", [])
                for pwr in pwrs:
                    powerup_dict[pwr["id"]] = pwr
                    
            elif msg_type == "tick":
                players = msg.get("players", {})
                new_other_snakes = {}
                for pid, pdata in players.items():
                    if pid == net_client.client_id:
                        continue
                    new_other_snakes[pid] = pdata
                other_snakes = new_other_snakes
                
            elif msg_type == "food_spawn":
                foods = msg.get("foods", [])
                for f in foods:
                    food_dict[f["id"]] = f
                    
            elif msg_type == "food_eaten":
                fid = msg.get("id")
                if fid in food_dict:
                    f = food_dict[fid]
                    particles.append(Particle(f["x"], f["y"], f["color"], "shockwave"))
                    for _ in range(4):
                        particles.append(Particle(f["x"], f["y"], f["color"], "spark"))
                    del food_dict[fid]
                    
            elif msg_type == "powerup_spawn":
                pwrs = msg.get("powerups", [])
                for pwr in pwrs:
                    powerup_dict[pwr["id"]] = pwr
                    
            elif msg_type == "powerup_eaten":
                pid = msg.get("id")
                if pid in powerup_dict:
                    pwr = powerup_dict[pid]
                    p_color = NEON_CYAN if pwr["type"] == "shield" else (NEON_PURPLE if pwr["type"] == "magnet" else NEON_YELLOW)
                    for _ in range(12):
                        particles.append(Particle(pwr["x"], pwr["y"], p_color))
                    del powerup_dict[pid]
                    
            elif msg_type == "apply_powerup":
                pwr_type = msg.get("powerup")
                if pwr_type == "shield":
                    powerup_shield_timer = 8.0
                    active_toasts.append({"text": "獲得 🛡️ 鐵壁防護罩狀態 (8秒)！", "color": NEON_CYAN, "life": 2.5})
                elif pwr_type == "magnet":
                    powerup_magnet_timer = 8.0
                    active_toasts.append({"text": "獲得 🧲 超載磁鐵吸食狀態 (8秒)！", "color": NEON_PURPLE, "life": 2.5})
                elif pwr_type == "speed":
                    powerup_speed_timer = 8.0
                    active_toasts.append({"text": "獲得 ⚡ 噴射加速推進狀態 (8秒)！", "color": NEON_YELLOW, "life": 2.5})

            elif msg_type == "skill_state":
                pid = msg.get("id")
                active = msg.get("active")
                if pid == net_client.client_id:
                    pass
                elif pid in other_snakes:
                    other_snakes[pid]["skill_active"] = active

            elif msg_type == "player_die":
                pid = msg.get("id")
                if pid == net_client.client_id:
                    is_dead = True
                elif pid in other_snakes:
                    dead_snake = other_snakes[pid]
                    d_color = dead_snake.get("color", (100, 100, 100))
                    for seg in dead_snake.get("body", []):
                        for _ in range(3):
                            particles.append(Particle(seg[0], seg[1], d_color))
                    del other_snakes[pid]

        if not net_client.connected:
            is_dead = True
            error_msg = "與伺服器斷開連線。"

        # Inputs
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return "quit"
            elif event.type == pygame.VIDEORESIZE:
                global SCREEN_WIDTH, SCREEN_HEIGHT, screen
                SCREEN_WIDTH, SCREEN_HEIGHT = event.size
                screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.RESIZABLE)
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    return "lobby"
                elif event.key == pygame.K_r and is_dead:
                    # Respawn
                    is_dead = False
                    snake_x = random.randint(500, MAP_SIZE - 500)
                    snake_y = random.randint(500, MAP_SIZE - 500)
                    snake_angle = random.uniform(0, 2 * math.pi)
                    length = 25 if p_class == "guardian" else 15
                    score = length
                    path_history = [(snake_x, snake_y)]
                    for i in range(1, length * SPACING):
                        path_history.append((snake_x - math.cos(snake_angle) * i, snake_y - math.sin(snake_angle) * i))
                    local_body = update_body_segments(path_history, length, SPACING)
                    
                    skill_active = False
                    skill_active_timer = 0
                    skill_cooldown_timer = 0
                    powerup_shield_timer = 0
                    powerup_magnet_timer = 0
                    powerup_speed_timer = 0
                    
                    net_client.send({
                        "type": "join",
                        "name": player_name,
                        "color": color,
                        "class": p_class,
                        "body": local_body
                    })
                elif event.key == pygame.K_w and not is_dead:
                    if skill_cooldown_timer <= 0:
                        skill_active = True
                        skill_active_timer = durations[p_class]
                        skill_cooldown_timer = cooldowns[p_class]
                        net_client.send({"type": "skill_activate"})
                        skill_names_tc = {"hunter": "幽靈突襲 🔴", "guardian": "鐵壁充能 🟢", "speedster": "磁場超載 🔵"}
                        active_toasts.append({"text": f"啟動 {skill_names_tc[p_class]} 主動技能 ({durations[p_class]}秒)！", "color": color, "life": 2.5})

            elif event.type == pygame.MOUSEBUTTONDOWN:
                mx, my = event.pos
                
                # If dead, check buttons clicks
                if is_dead:
                    # Button 1: Respawn (重新復活)
                    if SCREEN_WIDTH // 2 - 210 <= mx <= SCREEN_WIDTH // 2 - 80 and SCREEN_HEIGHT // 2 + 40 <= my <= SCREEN_HEIGHT // 2 + 80:
                        is_dead = False
                        snake_x = random.randint(500, MAP_SIZE - 500)
                        snake_y = random.randint(500, MAP_SIZE - 500)
                        snake_angle = random.uniform(0, 2 * math.pi)
                        length = 25 if p_class == "guardian" else 15
                        score = length
                        path_history = [(snake_x, snake_y)]
                        for i in range(1, length * SPACING):
                            path_history.append((snake_x - math.cos(snake_angle) * i, snake_y - math.sin(snake_angle) * i))
                        local_body = update_body_segments(path_history, length, SPACING)
                        
                        skill_active = False
                        skill_active_timer = 0
                        skill_cooldown_timer = 0
                        powerup_shield_timer = 0
                        powerup_magnet_timer = 0
                        powerup_speed_timer = 0
                        
                        net_client.send({
                            "type": "join",
                            "name": player_name,
                            "color": color,
                            "class": p_class,
                            "body": local_body
                        })
                    
                    # Button 2: Return to Lobby (返回大廳)
                    elif SCREEN_WIDTH // 2 - 70 <= mx <= SCREEN_WIDTH // 2 + 70 and SCREEN_HEIGHT // 2 + 40 <= my <= SCREEN_HEIGHT // 2 + 80:
                        return "lobby"
                        
                    # Button 3: Exit Game (離開遊戲)
                    elif SCREEN_WIDTH // 2 + 80 <= mx <= SCREEN_WIDTH // 2 + 210 and SCREEN_HEIGHT // 2 + 40 <= my <= SCREEN_HEIGHT // 2 + 80:
                        return "quit"
                else:
                    if event.button == 3: # Right click triggers skill
                        if skill_cooldown_timer <= 0:
                            skill_active = True
                            skill_active_timer = durations[p_class]
                            skill_cooldown_timer = cooldowns[p_class]
                            net_client.send({"type": "skill_activate"})
                            skill_names_tc = {"hunter": "幽靈突襲 🔴", "guardian": "鐵壁充能 🟢", "speedster": "磁場超載 🔵"}
                            active_toasts.append({"text": f"啟動 {skill_names_tc[p_class]} 主動技能 ({durations[p_class]}秒)！", "color": color, "life": 2.5})

        if not is_dead:
            mx, my = pygame.mouse.get_pos()
            camera_x = snake_x - SCREEN_WIDTH / 2
            camera_y = snake_y - SCREEN_HEIGHT / 2
            mouse_world_x = mx + camera_x
            mouse_world_y = my + camera_y
            
            dx = mouse_world_x - snake_x
            dy = mouse_world_y - snake_y
            dist = math.hypot(dx, dy)
            
            if dist > 5:
                target_angle = math.atan2(dy, dx)
                angle_diff = (target_angle - snake_angle + math.pi) % (2 * math.pi) - math.pi
                snake_angle += angle_diff * (0.15 * turn_factor)

            keys = pygame.key.get_pressed()
            mouse_buttons = pygame.mouse.get_pressed()
            is_boosting = (keys[pygame.K_SPACE] or mouse_buttons[0]) and length > MIN_BOOST_LENGTH
            
            base_s = BASE_SPEED * speed_factor
            boost_s = BOOST_SPEED * speed_factor
            
            if powerup_speed_timer > 0:
                base_s *= 1.5
                boost_s *= 1.5
                is_boosting = True
                
            if skill_active and p_class == "speedster":
                base_s *= 1.6
                boost_s *= 1.6
                is_boosting = True
                
            speed = boost_s if is_boosting else base_s
            
            snake_x += math.cos(snake_angle) * speed
            snake_y += math.sin(snake_angle) * speed
            
            snake_x = max(10, min(MAP_SIZE - 10, snake_x))
            snake_y = max(10, min(MAP_SIZE - 10, snake_y))

            path_history.insert(0, (snake_x, snake_y))
            path_history = path_history[:length * SPACING * 2]
            local_body = update_body_segments(path_history, length, SPACING)

            if is_boosting and powerup_speed_timer <= 0 and not (skill_active and p_class == "speedster"):
                boost_frame_counter += 1
                drain_rate = SHRINK_RATE * 2 if p_class == "guardian" else SHRINK_RATE
                if boost_frame_counter >= drain_rate:
                    boost_frame_counter = 0
                    length -= 1
                    score = length
                    
                    tail_pos = local_body[-1]
                    net_client.send({
                        "type": "boost_drop",
                        "x": tail_pos[0] + random.uniform(-10, 10),
                        "y": tail_pos[1] + random.uniform(-10, 10),
                        "color": color
                    })
                    particles.append(Particle(tail_pos[0], tail_pos[1], color))
            
            if is_boosting or skill_active:
                tail_pos = local_body[-1]
                particles.append(Particle(tail_pos[0] + random.uniform(-8, 8), tail_pos[1] + random.uniform(-8, 8), color, "spark"))

            head = local_body[0]
            magnet_range = 16
            if powerup_magnet_timer > 0:
                magnet_range += 180
            if skill_active and p_class == "speedster":
                magnet_range += 250
            if p_class == "hunter":
                magnet_range += 8

            eaten_ids = []
            for fid, f in list(food_dict.items()):
                fdx = f["x"] - head[0]
                fdy = f["y"] - head[1]
                f_dist = math.hypot(fdx, fdy)
                
                if f_dist < magnet_range and magnet_range > 30:
                    pull_speed = 12
                    angle_to_head = math.atan2(head[1] - f["y"], head[0] - f["x"])
                    f["x"] += math.cos(angle_to_head) * pull_speed
                    f["y"] += math.sin(angle_to_head) * pull_speed
                    
                    if tick_count % 3 == 0:
                        particles.append(Particle(f["x"], f["y"], (255, 255, 255), "spark"))

                if f_dist < (16 + f["size"]):
                    eaten_ids.append(fid)
                    grow_amt = 1
                    if p_class == "hunter":
                        grow_amt = 2 if f["size"] >= 8 else 1
                    
                    length += grow_amt
                    score = length
                    
                    particles.append(Particle(f["x"], f["y"], f["color"], "shockwave"))
                    for _ in range(4):
                        particles.append(Particle(f["x"], f["y"], f["color"], "spark"))
                    del food_dict[fid]
            
            for fid in eaten_ids:
                net_client.send({"type": "eat", "id": fid})

            is_invincible = (powerup_shield_timer > 0) or (skill_active and p_class in ["hunter", "guardian"])
            
            if not is_invincible:
                for pid, other_snake in other_snakes.items():
                    if other_snake.get("skill_active") and other_snake.get("class") == "hunter":
                        continue
                        
                    other_body = other_snake.get("body", [])
                    for seg in other_body:
                        c_dist = math.hypot(seg[0] - head[0], seg[1] - head[1])
                        if c_dist < 26:
                            is_dead = True
                            net_client.send({"type": "die"})
                            for s_pos in local_body:
                                for _ in range(3):
                                    particles.append(Particle(s_pos[0], s_pos[1], color))
                            break
                    if is_dead:
                        break

            current_time = time.time()
            if current_time - last_update_time >= update_interval:
                net_client.send({
                    "type": "update",
                    "score": score,
                    "body": local_body
                })
                last_update_time = current_time

        for p in list(particles):
            p.update()
            if p.life <= 0:
                particles.remove(p)

        # Determine who is the King (highest score player/bot in the arena)
        king_score = 0
        king_id = None
        if not is_dead:
            king_score = score
            king_id = "local"
        for pid, other in other_snakes.items():
            if other.get("score", 0) > king_score:
                king_score = other["score"]
                king_id = pid

        # Rendering
        screen.fill(BG_COLOR)
        camera_x = snake_x - SCREEN_WIDTH / 2
        camera_y = snake_y - SCREEN_HEIGHT / 2
        
        # Parallax
        for star in PARALLAX_STARS:
            sx = int((star["x"] - camera_x * 0.4) % SCREEN_WIDTH)
            sy = int((star["y"] - camera_y * 0.4) % SCREEN_HEIGHT)
            pygame.draw.circle(screen, star["color"], (sx, sy), star["size"])

        # Grid
        start_x = int(camera_x - (camera_x % GRID_SIZE))
        start_y = int(camera_y - (camera_y % GRID_SIZE))
        for gx in range(start_x, start_x + SCREEN_WIDTH + GRID_SIZE, GRID_SIZE):
            pygame.draw.line(screen, GRID_COLOR, (gx - camera_x, 0), (gx - camera_x, SCREEN_HEIGHT))
        for gy in range(start_y, start_y + SCREEN_HEIGHT + GRID_SIZE, GRID_SIZE):
            pygame.draw.line(screen, GRID_COLOR, (0, gy - camera_y), (SCREEN_WIDTH, gy - camera_y))

        # Borders
        rx = 0 - camera_x
        ry = 0 - camera_y
        pygame.draw.rect(screen, BORDER_COLOR, (rx, ry, MAP_SIZE, MAP_SIZE), width=12)

        # Powerups
        for pid, p in powerup_dict.items():
            px = int(p["x"] - camera_x)
            py = int(p["y"] - camera_y)
            if -50 <= px <= SCREEN_WIDTH + 50 and -50 <= py <= SCREEN_HEIGHT + 50:
                p_color = NEON_CYAN if p["type"] == "shield" else (NEON_PURPLE if p["type"] == "magnet" else NEON_YELLOW)
                c_size = 18 + int(math.sin(tick_count * 0.12) * 3)
                
                # Outer glowing aura ring
                pygame.draw.circle(screen, p_color, (px, py), c_size + 4, width=1)
                
                # Rotating Hexagonal Crystal outline
                angle_offset = tick_count * 0.04
                hex_points = []
                for idx in range(6):
                    angle = angle_offset + idx * (math.pi / 3)
                    hex_points.append((px + c_size * math.cos(angle), py + c_size * math.sin(angle)))
                
                # Draw filled semi-transparent crystal body
                pygame.draw.polygon(screen, p_color, hex_points, width=2)
                pygame.draw.polygon(screen, (p_color[0]//4, p_color[1]//4, p_color[2]//4), hex_points)
                
                # Draw Custom Icon Emblem inside the crystal
                cx, cy = px, py
                if p["type"] == "shield":
                    # Draw a nice shield emblem
                    shield_pts = [(cx - 7, cy - 8), (cx + 7, cy - 8), (cx + 7, cy + 1), (cx, cy + 9), (cx - 7, cy + 1)]
                    pygame.draw.polygon(screen, p_color, shield_pts, width=1)
                    pygame.draw.line(screen, p_color, (cx, cy - 5), (cx, cy + 5), width=2)
                    pygame.draw.line(screen, p_color, (cx - 4, cy - 1), (cx + 4, cy - 1), width=2)
                elif p["type"] == "magnet":
                    # Draw a detailed horseshoe magnet U-shape
                    pygame.draw.rect(screen, p_color, (cx - 6, cy - 6, 4, 11), border_radius=1)
                    pygame.draw.rect(screen, p_color, (cx + 2, cy - 6, 4, 11), border_radius=1)
                    pygame.draw.rect(screen, p_color, (cx - 6, cy + 2, 12, 3), border_radius=1)
                    # Red positive/negative pole tips on top
                    pygame.draw.rect(screen, (255, 50, 50), (cx - 6, cy - 8, 4, 3))
                    pygame.draw.rect(screen, (50, 150, 255), (cx + 2, cy - 8, 4, 3))
                elif p["type"] == "speed":
                    # Draw a detailed lightning bolt
                    bolt_pts = [
                        (cx - 2, cy - 9), (cx + 4, cy - 9), (cx + 1, cy - 2), 
                        (cx + 6, cy - 2), (cx - 4, cy + 9), (cx - 1, cy + 1), (cx - 4, cy + 1)
                    ]
                    pygame.draw.polygon(screen, p_color, bolt_pts)

        # Food
        for f in food_dict.values():
            fx = int(f["x"] - camera_x)
            fy = int(f["y"] - camera_y)
            if -50 <= fx <= SCREEN_WIDTH + 50 and -50 <= fy <= SCREEN_HEIGHT + 50:
                # Glowing Cyber Halo
                pygame.draw.circle(screen, f["color"], (fx, fy), f["size"] + 3, width=1)
                pygame.draw.circle(screen, f["color"], (fx, fy), f["size"])

        # Other Snakes
        for pid, other in other_snakes.items():
            obody = other.get("body", [])
            ocolor = other.get("color", (100, 100, 100))
            oname = other.get("name", "Player")
            oclass = other.get("class", "hunter")
            other_active = other.get("skill_active", False)
            
            if len(obody) == 0:
                continue

            o_alpha = 90 if (other_active and oclass == "hunter") else 255
            
            for seg in reversed(obody[1:]):
                sx = int(seg[0] - camera_x)
                sy = int(seg[1] - camera_y)
                if -35 <= sx <= SCREEN_WIDTH + 35 and -35 <= sy <= SCREEN_HEIGHT + 35:
                    s = pygame.Surface((24, 24), pygame.SRCALPHA)
                    pygame.draw.circle(s, (*ocolor, o_alpha), (12, 12), 12)
                    # Glossy 3D Highlight Reflection
                    pygame.draw.circle(s, (255, 255, 255, int(o_alpha * 0.5)), (12 - 4, 12 - 4), 3)
                    screen.blit(s, (sx - 12, sy - 12))
                    
            hx = int(obody[0][0] - camera_x)
            hy = int(obody[0][1] - camera_y)
            if -35 <= hx <= SCREEN_WIDTH + 35 and -35 <= hy <= SCREEN_HEIGHT + 35:
                s = pygame.Surface((36, 36), pygame.SRCALPHA)
                pygame.draw.circle(s, (*ocolor, o_alpha), (18, 18), 16)
                # Glossy 3D Highlight Reflection
                pygame.draw.circle(s, (255, 255, 255, int(o_alpha * 0.6)), (18 - 5, 18 - 5), 4)
                screen.blit(s, (hx - 18, hy - 18))
                
                class_map_tc = {"hunter": "掠奪者", "guardian": "守衛者", "speedster": "風暴使者"}
                class_tag = f" [{class_map_tc.get(oclass, '掠奪者')}]"
                name_surf = font_small.render(oname + class_tag, True, TEXT_COLOR)
                screen.blit(name_surf, (hx - name_surf.get_width() // 2, hy - 32))
                
                # Draw King's golden crown badge
                if king_id == pid:
                    king_surf = font_bold_small.render("👑 競技場霸主", True, NEON_YELLOW)
                    screen.blit(king_surf, (hx - king_surf.get_width() // 2, hy - 50))

                if other_active and oclass == "guardian":
                    draw_shield(screen, (hx, hy), NEON_GREEN, tick_count)
                
                if len(obody) > 1:
                    odx = obody[0][0] - obody[1][0]
                    ody = obody[0][1] - obody[1][1]
                    oang = math.atan2(ody, odx)
                    
                    eye_dist = 8
                    lx = hx + math.cos(oang + 0.5) * eye_dist
                    ly = hy + math.sin(oang + 0.5) * eye_dist
                    pygame.draw.circle(screen, (255, 255, 255), (int(lx), int(ly)), 4)
                    pygame.draw.circle(screen, (0, 0, 0), (int(lx), int(ly)), 2)
                    rx = hx + math.cos(oang - 0.5) * eye_dist
                    ry = hy + math.sin(oang - 0.5) * eye_dist
                    pygame.draw.circle(screen, (255, 255, 255), (int(rx), int(ry)), 4)
                    pygame.draw.circle(screen, (0, 0, 0), (int(rx), int(ry)), 2)

        # Local Snake
        if not is_dead:
            my_alpha = 100 if (skill_active and p_class == "hunter") else 255
            
            for i, seg in enumerate(reversed(local_body[1:])):
                sx = int(seg[0] - camera_x)
                sy = int(seg[1] - camera_y)
                
                seg_idx = len(local_body) - 1 - i
                scale_ratio = 1.0 - (seg_idx / len(local_body)) * 0.3
                radius = int(12 * scale_ratio)
                
                if -35 <= sx <= SCREEN_WIDTH + 35 and -35 <= sy <= SCREEN_HEIGHT + 35:
                    s = pygame.Surface((radius * 2, radius * 2), pygame.SRCALPHA)
                    pygame.draw.circle(s, (*color, my_alpha), (radius, radius), radius)
                    pygame.draw.circle(s, (*color, int(my_alpha * 0.3)), (radius, radius), radius, width=1)
                    # Glossy 3D Highlight Reflection
                    reflect_offset = int(radius * 0.3)
                    reflect_radius = max(2, int(radius * 0.22))
                    pygame.draw.circle(s, (255, 255, 255, int(my_alpha * 0.5)), (radius - reflect_offset, radius - reflect_offset), reflect_radius)
                    screen.blit(s, (sx - radius, sy - radius))

            hx = int(local_body[0][0] - camera_x)
            hy = int(local_body[0][1] - camera_y)
            
            s = pygame.Surface((36, 36), pygame.SRCALPHA)
            pygame.draw.circle(s, (*color, my_alpha), (18, 18), 16)
            pygame.draw.circle(s, (255, 255, 255, int(my_alpha * 0.8)), (18, 18), 18, width=1)
            # Glossy 3D Highlight Reflection
            pygame.draw.circle(s, (255, 255, 255, int(my_alpha * 0.6)), (18 - 5, 18 - 5), 4)
            screen.blit(s, (hx - 18, hy - 18))
            
            name_surf = font_bold_small.render(player_name, True, NEON_CYAN)
            screen.blit(name_surf, (hx - name_surf.get_width() // 2, hy - 32))
            
            # Draw King's golden crown badge
            if king_id == "local":
                king_surf = font_bold_small.render("👑 競技場霸主", True, NEON_YELLOW)
                screen.blit(king_surf, (hx - king_surf.get_width() // 2, hy - 50))

            if (skill_active and p_class == "guardian") or (powerup_shield_timer > 0):
                shield_color = NEON_CYAN if (powerup_shield_timer > 0) else NEON_GREEN
                draw_shield(screen, (hx, hy), shield_color, tick_count)

            if len(local_body) > 1:
                lx = hx + math.cos(snake_angle + 0.5) * 8
                ly = hy + math.sin(snake_angle + 0.5) * 8
                pygame.draw.circle(screen, (255, 255, 255), (int(lx), int(ly)), 4)
                pygame.draw.circle(screen, (0, 0, 0), (int(lx), int(ly)), 2)
                rx = hx + math.cos(snake_angle - 0.5) * 8
                ry = hy + math.sin(snake_angle - 0.5) * 8
                pygame.draw.circle(screen, (255, 255, 255), (int(rx), int(ry)), 4)
                pygame.draw.circle(screen, (0, 0, 0), (int(rx), int(ry)), 2)

        for p in particles:
            p.draw(screen, camera_x, camera_y)

        # --- DRAW UI HUD ---
        # 1. Score HUD
        hud_w, hud_h = 200, 110
        hud_surface = pygame.Surface((hud_w, hud_h), pygame.SRCALPHA)
        pygame.draw.rect(hud_surface, (28, 28, 38, 180), (0, 0, hud_w, hud_h), border_radius=10)
        pygame.draw.rect(hud_surface, (0, 255, 204, 100), (0, 0, hud_w, hud_h), width=1, border_radius=10)
        screen.blit(hud_surface, (20, SCREEN_HEIGHT - 130))
        
        score_lbl = font_medium.render(f"得分: {score}", True, TEXT_COLOR)
        screen.blit(score_lbl, (35, SCREEN_HEIGHT - 120))
        
        class_map = {"hunter": "🔴 掠奪者", "guardian": "🟢 守衛者", "speedster": "🔵 風暴使者"}
        class_lbl = font_bold_small.render(f"職業: {class_map[p_class]}", True, color)
        screen.blit(class_lbl, (35, SCREEN_HEIGHT - 95))
        
        pos_lbl = font_small.render(f"位置: {int(snake_x)}, {int(snake_y)}", True, (150, 150, 160))
        screen.blit(pos_lbl, (35, SCREEN_HEIGHT - 70))

        # 2. ACTIVE SKILL HUD (Visual Progress Bar)
        sk_w, sk_h = 240, 80
        sk_x = SCREEN_WIDTH // 2 - sk_w // 2
        sk_y = SCREEN_HEIGHT - 100
        
        sk_surface = pygame.Surface((sk_w, sk_h), pygame.SRCALPHA)
        pygame.draw.rect(sk_surface, (28, 28, 38, 190), (0, 0, sk_w, sk_h), border_radius=10)
        pygame.draw.rect(sk_surface, (255, 220, 50, 80), (0, 0, sk_w, sk_h), width=1, border_radius=10)
        screen.blit(sk_surface, (sk_x, sk_y))
        
        skill_names = {
            "hunter": "幽靈突襲 (Ghost Dash)",
            "guardian": "鐵壁充能 (Iron Barrier)",
            "speedster": "磁場超載 (Magnetic Overload)"
        }
        
        s_lbl = font_bold_small.render(skill_names[p_class], True, NEON_YELLOW)
        screen.blit(s_lbl, (sk_x + 15, sk_y + 10))
        
        if skill_active:
            status_txt = f"持續中 ({skill_active_timer:.1f}秒)"
            status_color = NEON_CYAN
        elif skill_cooldown_timer > 0:
            status_txt = f"冷卻中 ({skill_cooldown_timer:.1f}秒)"
            status_color = NEON_RED
        else:
            status_txt = "準備就緒 (滑鼠右鍵 / W)"
            status_color = NEON_GREEN
            
        stat_surf = font_small.render(status_txt, True, status_color)
        screen.blit(stat_surf, (sk_x + 15, sk_y + 30))
        
        # Draw Visual Progress Bar
        bar_x = sk_x + 15
        bar_y = sk_y + 56
        bar_w = sk_w - 30
        bar_h = 8
        
        pygame.draw.rect(screen, (50, 50, 60), (bar_x, bar_y, bar_w, bar_h), border_radius=4)
        if skill_active:
            pct = skill_active_timer / durations[p_class]
            pygame.draw.rect(screen, NEON_CYAN, (bar_x, bar_y, int(bar_w * pct), bar_h), border_radius=4)
            pygame.draw.rect(screen, (255, 255, 255), (bar_x, bar_y, int(bar_w * pct), bar_h), width=1, border_radius=4)
        elif skill_cooldown_timer > 0:
            pct = 1.0 - (skill_cooldown_timer / cooldowns[p_class])
            pygame.draw.rect(screen, NEON_RED, (bar_x, bar_y, int(bar_w * pct), bar_h), border_radius=4)
        else:
            pygame.draw.rect(screen, NEON_GREEN, (bar_x, bar_y, bar_w, bar_h), border_radius=4)

        # Active Powerups
        active_items = []
        if powerup_shield_timer > 0: active_items.append(f"🛡️ 護盾能量: {powerup_shield_timer:.1f}秒")
        if powerup_magnet_timer > 0: active_items.append(f"🧲 超載磁鐵: {powerup_magnet_timer:.1f}秒")
        if powerup_speed_timer > 0: active_items.append(f"⚡ 噴射加速: {powerup_speed_timer:.1f}秒")
        
        iy = sk_y - 25
        for item in active_items:
            item_surf = font_bold_small.render(item, True, NEON_YELLOW)
            pygame.draw.rect(screen, (20, 20, 25, 200), (sk_x, iy, sk_w, 20), border_radius=5)
            screen.blit(item_surf, (sk_x + 15, iy + 2))
            iy -= 22

        # 3. Leaderboard HUD
        lb_w, lb_h = 240, 200
        lb_surface = pygame.Surface((lb_w, lb_h), pygame.SRCALPHA)
        pygame.draw.rect(lb_surface, (28, 28, 38, 180), (0, 0, lb_w, lb_h), border_radius=10)
        pygame.draw.rect(lb_surface, (0, 255, 204, 100), (0, 0, lb_w, lb_h), width=1, border_radius=10)
        screen.blit(lb_surface, (SCREEN_WIDTH - 260, 20))
        
        lb_title = font_bold_small.render("排行榜 (Leaderboard)", True, NEON_CYAN)
        screen.blit(lb_title, (SCREEN_WIDTH - 260 + 120 - lb_title.get_width() // 2, 30))
        
        scores_list = []
        if not is_dead:
            scores_list.append((player_name, score, True, False))
        for other in other_snakes.values():
            scores_list.append((other["name"], other["score"], False, other.get("is_bot", False)))
            
        scores_list.sort(key=lambda x: x[1], reverse=True)
        
        y_offset = 55
        for idx, (name, s_val, is_me, is_bot) in enumerate(scores_list[:5]):
            rank_lbl = font_small.render(f"{idx+1}.", True, (150, 150, 160))
            name_color = NEON_CYAN if is_me else (NEON_RED if is_bot else TEXT_COLOR)
            display_name = name[:12] + (" (電腦)" if is_bot else "")
            p_lbl = font_small.render(display_name, True, name_color)
            val_lbl = font_bold_small.render(str(s_val), True, name_color)
            
            screen.blit(rank_lbl, (SCREEN_WIDTH - 245, 20 + y_offset))
            screen.blit(p_lbl, (SCREEN_WIDTH - 225, 20 + y_offset))
            screen.blit(val_lbl, (SCREEN_WIDTH - 60, 20 + y_offset))
            y_offset += 26

        # 4. RADAR MINIMAP
        mm_w, mm_h = 160, 160
        mm_x = SCREEN_WIDTH - 180
        mm_y = SCREEN_HEIGHT - 180
        
        mm_surface = pygame.Surface((mm_w, mm_h), pygame.SRCALPHA)
        pygame.draw.rect(mm_surface, (20, 20, 28, 190), (0, 0, mm_w, mm_h), border_radius=8)
        pygame.draw.rect(mm_surface, (0, 255, 204, 100), (0, 0, mm_w, mm_h), width=1, border_radius=8)
        screen.blit(mm_surface, (mm_x, mm_y))
        
        # Radar scanlines / grid for cyberpunk look
        for offset in range(20, 160, 40):
            pygame.draw.line(screen, (10, 40, 35), (mm_x + offset, mm_y), (mm_x + offset, mm_y + 160))
            pygame.draw.line(screen, (10, 40, 35), (mm_x, mm_y + offset), (mm_x + 160, mm_y + offset))
        
        scale = mm_w / MAP_SIZE
        
        for p in powerup_dict.values():
            px = int(p["x"] * scale)
            py = int(p["y"] * scale)
            pygame.draw.circle(screen, NEON_YELLOW, (mm_x + px, mm_y + py), 3)

        for pid, other in other_snakes.items():
            obody = other.get("body", [])
            if obody:
                ox = int(obody[0][0] * scale)
                oy = int(obody[0][1] * scale)
                if king_id == pid:
                    k_color = NEON_YELLOW if (int(tick_count * 0.25) % 2 == 0) else (255, 165, 0)
                    pygame.draw.circle(screen, k_color, (mm_x + ox, mm_y + oy), 5)
                    pygame.draw.circle(screen, (255, 255, 255), (mm_x + ox, mm_y + oy), 6, width=1)
                else:
                    dot_color = NEON_RED if other.get("is_bot") else (255, 120, 0)
                    pygame.draw.circle(screen, dot_color, (mm_x + ox, mm_y + oy), 2)

        if not is_dead:
            sx = int(snake_x * scale)
            sy = int(snake_y * scale)
            if king_id == "local":
                k_color = NEON_YELLOW if (int(tick_count * 0.25) % 2 == 0) else (255, 165, 0)
                pygame.draw.circle(screen, k_color, (mm_x + sx, mm_y + sy), 5)
                pygame.draw.circle(screen, (255, 255, 255), (mm_x + sx, mm_y + sy), 6, width=1)
            else:
                if int(tick_count * 0.1) % 2 == 0:
                    pygame.draw.circle(screen, NEON_CYAN, (mm_x + sx, mm_y + sy), 4)

        # 5. Game Over Screen Overlay with Interactive Clickable Buttons
        if is_dead:
            overlay = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.SRCALPHA)
            overlay.fill((0, 0, 0, 190))
            screen.blit(overlay, (0, 0))
            
            go_title = font_title.render("遊戲結束 (GAME OVER)", True, NEON_RED)
            screen.blit(go_title, (SCREEN_WIDTH // 2 - go_title.get_width() // 2, SCREEN_HEIGHT // 2 - 100))
            
            go_score = font_large.render(f"你的得分: {score}", True, TEXT_COLOR)
            screen.blit(go_score, (SCREEN_WIDTH // 2 - go_score.get_width() // 2, SCREEN_HEIGHT // 2 - 20))
            
            if error_msg:
                go_err = font_small.render(error_msg, True, NEON_RED)
                screen.blit(go_err, (SCREEN_WIDTH // 2 - go_err.get_width() // 2, SCREEN_HEIGHT // 2 + 105))
            
            # Draw Buttons
            # Button 1: Respawn (重新復活)
            b1_x = SCREEN_WIDTH // 2 - 210
            b_y = SCREEN_HEIGHT // 2 + 40
            b1_w, b_h = 130, 40
            # Detect Hover
            m_pos = pygame.mouse.get_pos()
            b1_color = (0, 255, 204) if (b1_x <= m_pos[0] <= b1_x + b1_w and b_y <= m_pos[1] <= b_y + b_h) else (0, 180, 150)
            pygame.draw.rect(screen, b1_color, (b1_x, b_y, b1_w, b_h), border_radius=8)
            b1_txt = font_medium.render("重新復活", True, BG_COLOR)
            screen.blit(b1_txt, (b1_x + b1_w // 2 - b1_txt.get_width() // 2, b_y + b_h // 2 - b1_txt.get_height() // 2))
            
            # Button 2: Return to Lobby (返回大廳)
            b2_x = SCREEN_WIDTH // 2 - 70
            b2_w = 140
            b2_color = (255, 220, 50) if (b2_x <= m_pos[0] <= b2_x + b2_w and b_y <= m_pos[1] <= b_y + b_h) else (200, 170, 40)
            pygame.draw.rect(screen, b2_color, (b2_x, b_y, b2_w, b_h), border_radius=8)
            b2_txt = font_medium.render("返回大廳重選", True, BG_COLOR)
            screen.blit(b2_txt, (b2_x + b2_w // 2 - b2_txt.get_width() // 2, b_y + b_h // 2 - b2_txt.get_height() // 2))
            
            # Button 3: Exit Game (離開遊戲)
            b3_x = SCREEN_WIDTH // 2 + 80
            b3_w = 130
            b3_color = (255, 60, 100) if (b3_x <= m_pos[0] <= b3_x + b3_w and b_y <= m_pos[1] <= b_y + b_h) else (200, 40, 80)
            pygame.draw.rect(screen, b3_color, (b3_x, b_y, b3_w, b_h), border_radius=8)
            b3_txt = font_medium.render("離開遊戲", True, TEXT_COLOR)
            screen.blit(b3_txt, (b3_x + b3_w // 2 - b3_txt.get_width() // 2, b_y + b_h // 2 - b3_txt.get_height() // 2))

        # Render On-screen Toast Notifications
        ty = 130
        for toast in active_toasts:
            alpha = int(min(1.0, toast["life"]) * 255)
            txt_surf = font_medium.render(toast["text"], True, toast["color"])
            
            # Glassmorphic Box
            t_w = txt_surf.get_width() + 40
            t_h = txt_surf.get_height() + 16
            t_surf = pygame.Surface((t_w, t_h), pygame.SRCALPHA)
            pygame.draw.rect(t_surf, (22, 22, 30, int(alpha * 0.8)), (0, 0, t_w, t_h), border_radius=12)
            pygame.draw.rect(t_surf, (*toast["color"], int(alpha * 0.9)), (0, 0, t_w, t_h), width=2, border_radius=12)
            
            # Text transparency
            txt_alpha_surf = txt_surf.copy()
            txt_alpha_surf.set_alpha(alpha)
            t_surf.blit(txt_alpha_surf, (20, 8))
            
            # Blit to screen
            screen.blit(t_surf, (SCREEN_WIDTH // 2 - t_w // 2, ty))
            ty += t_h + 12

        pygame.display.flip()

# Main state machine
def main():
    player_name = f"Snake_{random.randint(100, 999)}"
    server_address = "wss://ntut-ee-snake-server.johnsontw102060.workers.dev/ws"
    selected_class = "hunter"
    
    while True:
        # Run Lobby
        lobby_res = run_lobby_screen(player_name, server_address, selected_class)
        if lobby_res == "quit":
            break
            
        player_name, server_address, selected_class, net_client = lobby_res
        
        # Run Gameplay
        game_res = run_gameplay(player_name, server_address, selected_class, net_client)
        if game_res == "quit":
            break
        # If game_res is "lobby", loop will continue back to run_lobby_screen!
        
    if 'net_client' in locals() and net_client:
        net_client.close()
    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    main()
