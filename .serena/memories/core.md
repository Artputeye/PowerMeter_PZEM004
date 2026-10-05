# Project Core

- ESP32 PowerMeter project using the Hybrid-Inverter architecture pattern: `main.cpp` owns FreeRTOS task startup; `app_main` coordinates application logic; hardware, network, HTTP, WebSocket, storage, logging, time sync, and OTA are isolated modules.
- PZEM-004T v3.0 is the primary measurement source; web telemetry is exposed through `/api/pzem` and WebSocket `/ws`.
- LittleFS serves the web UI from `data/`; OTA uses the project's OTA partition table.
- Build configuration and dependency pins are in `platformio.ini`.