# Tech Stack

- ESP32 Dev Module, Arduino framework, PlatformIO espressif32 ~6.0.0.
- PZEM library: `mandulaj/PZEM-004T-v30`.
- Async web stack: ESPAsyncWebServer + AsyncTCP.
- WiFi provisioning: WiFiManager ^2.0.17.
- JSON: ArduinoJson ^7.4.1.
- Filesystem: LittleFS; partition file: `partitions/ota.csv`.
- PZEM UART: Serial2, RX GPIO16, TX GPIO17, 9600 baud 8N1.
- NTP: GMT+7 via pool.ntp.org.