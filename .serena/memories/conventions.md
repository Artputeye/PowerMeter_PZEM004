# Conventions

- Preserve the module separation inherited from Hybrid-Inverter: `main.cpp` should remain orchestration-only; feature logic belongs in its module.
- PZEM acquisition belongs in `pzem_monitor.*`; network lifecycle in `network_manager.*`; HTTP routes in `http_server.*`; WebSocket telemetry in `websocket_handler.*`; persistence in `storage_manager.*`; system logging/time/OTA in their respective modules.
- Web UI assets belong in `data/`; firmware sources belong in `src/`.
- Keep PZEM pin definitions in `config.h` so hardware wiring can be changed without touching measurement logic.