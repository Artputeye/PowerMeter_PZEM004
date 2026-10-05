// config.cpp
#include "config.h"

// --- Global Objects ---
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
PZEM004Tv30 pzem(Serial2, PZEM_RX_PIN, PZEM_TX_PIN);

// --- Device Info ---
char D_SoftwareVersion[15] = "1.0.1";
char D_Mfac[15] = "ARTTECH";
char D_Model[15] = "PowerMeter";

// --- Network Settings ---
char DEVICE_NAME[28] = "PowerMeter-PZEM004";
char DEVICE_PASS[28] = "12345678";
char WIFI_SSID[30] = "";
char WIFI_PASS[25] = "";
char HOSTNAME[30] = "powermeter";

// --- Static IP Settings ---
char DIVICE_IP[16] = "0.0.0.0";
char IP_ADDR[16] = "0.0.0.0";
char SUBNET_MASK[16] = "255.255.255.0";
char GATEWAY[16] = "0.0.0.0";

// --- MQTT Settings ---
char MQTT_SERVER[64] = "192.168.1.100";
char MQTT_USER[32] = "powermeter";
char MQTT_PASS[32] = "12345678";
uint16_t MQTT_PORT = 1883;

// --- Auth & Status ---
char AUTH_USER[10] = "admin";
char AUTH_PASS[10] = "12345678";

bool isWifiApMode = false;
String deviceName = "PowerMeter-PZEM004";
