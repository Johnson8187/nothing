@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo ===================================================
echo             賽博英雄貪吃蛇 一鍵安裝與運行器
echo ===================================================
echo.

:: 1. Check Python
where python >nul 2>nul
if %errorlevel% neq 0 (
    echo [提示] 系統未偵測到 Python，將自動為您打開 Python 官網下載頁面...
    echo.
    echo 重要提示：在安裝 Python 時，請務必勾選下方框框：
    echo           "[x] Add Python.exe to PATH"
    echo 安裝完成後，請關閉此視窗並重新打開本程式。
    echo.
    start https://www.python.org/downloads/
    pause
    exit /b 1
)

:: 2. Download client.py and requirements.txt using python or curl
echo [提示] 偵測到 Python 環境。正在下載最新版遊戲檔案...
python -c "import urllib.request, ssl; req = urllib.request.Request('https://ntut-ee-snake-server.johnsontw102060.workers.dev/client.py', headers={'User-Agent': 'Mozilla/5.0'}); context = ssl._create_unverified_context(); res = urllib.request.urlopen(req, context=context); open('client.py', 'wb').write(res.read())"
if %errorlevel% neq 0 (
    echo [警告] 無法透過 Python 下載 client.py，嘗試使用 curl 下載...
    curl -k --ssl-no-revoke -L -o client.py "https://ntut-ee-snake-server.johnsontw102060.workers.dev/client.py"
)

python -c "import urllib.request, ssl; req = urllib.request.Request('https://ntut-ee-snake-server.johnsontw102060.workers.dev/requirements.txt', headers={'User-Agent': 'Mozilla/5.0'}); context = ssl._create_unverified_context(); res = urllib.request.urlopen(req, context=context); open('requirements.txt', 'wb').write(res.read())"
if %errorlevel% neq 0 (
    echo [警告] 無法透過 Python 下載 requirements.txt，嘗試使用 curl 下載...
    curl -k --ssl-no-revoke -L -o requirements.txt "https://ntut-ee-snake-server.johnsontw102060.workers.dev/requirements.txt"
)

:: 3. Install requirements
echo 正在安裝/升級遊戲所需的 Python 套件 (pygame, websocket-client)...
python -m pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo [警告] 依賴安裝失敗，嘗試個別安裝套件...
    python -m pip install pygame websocket-client
)

:: 4. Start game
echo.
echo ===================================================
echo   配置完畢！正在進入「賽博英雄貪吃蛇」...
echo ===================================================
echo.

python client.py
if %errorlevel% neq 0 (
    echo.
    echo [錯誤] 遊戲發生異常終止，請查看上方錯誤內容。
    pause
)
