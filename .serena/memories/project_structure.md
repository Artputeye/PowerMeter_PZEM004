# PowerMeter_PZEM004 Project Structure

Project root: D:\@Project\PowerMeter_PZEM004

## Main directories
- src/ : ESP32 firmware source and headers.
- data/ : LittleFS web UI assets (HTML/CSS/JS and images).
- include/ : additional include files.
- lib/ : local libraries.
- partitions/ : ESP32 partition definitions; partitions/ota.csv is used by PlatformIO.
- logs/ : build/logger build logs; keep build logs out of project root.
- tools/tmp/ : temporary Python scripts; keep tmp scripts out of project root.
- .serena/ : Serena project configuration and memories.
- .pio/ : PlatformIO generated/build artifacts.
- .vscode/ : VS Code configuration.

## Important root files
- platformio.ini : PlatformIO configuration.
- README.md : project documentation.
- design.md : design notes.
- history.md : project history.
- run-serena-tunnel.ps1 / run-serena-tunnel.bat : OpenAI Tunnel + Serena MCP startup scripts.

## Firmware modules
- src/main.cpp : ESP32 entry point, setup/loop, FreeRTOS tasks.
- src/app_main.cpp : app_setup/app_loop.
- src/config.cpp/.h : global configuration/shared variables.
- src/pzem_monitor.cpp/.h : PZEM-004T initialization, updates, data access.
- src/energy_history.cpp/.h : energy history initialization, updates, power records, JSON generation.
- src/expense_manager.cpp/.h : electricity-cost settings, progressive grid cost, estimated bill, JSON.
- src/storage_manager.cpp/.h : LittleFS storage, JSON/file operations, file management.
- src/network_manager.cpp/.h : Wi-Fi setup, keep-alive, AP mode.
- src/http_server.cpp/.h : HTTP server and web routes/content handling.
- src/websocket_handler.cpp/.h : WebSocket initialization, client notifications, processing.
- src/ha_integration.cpp/.h : MQTT/Home Assistant setup, discovery, state publishing/reconnect.
- src/ota_update.cpp/.h : firmware and LittleFS OTA uploads.
- src/time_sync.cpp/.h : NTP time synchronization.
- src/logger.cpp/.h : logging, reset-reason logging, log display/clear.
- src/ui_indicator.cpp/.h : LED indicator modes/patterns.

## Web UI
data/ contains modules/pages such as index, dashboard, setting, network, console, filelist, and ota, generally with matching .html/.js/.css files.

## Working conventions
- Keep root clean: build logs go in logs/, temporary scripts go in tools/tmp/.
- Use Serena semantic tools for C/C++ navigation/editing.
- For C/C++ work, use clangd via Serena.
- Serena project.local.yml configures cpp to use C:\Program Files\LLVM\bin\clangd.exe.
- Active project: PowerMeter_PZEM004; active language server: cpp.
