#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <Update.h>
#include <time.h>
#include <PZEM004Tv30.h>

#define FILESYSTEM LittleFS
#define STATUS_LED 2
#define AP_PIN 0
#define WDT_TIMEOUT 120

// PZEM-004T v3.0 UART connection
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17

// --- Global Objects ---
extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern PZEM004Tv30 pzem;

// --- Device Info ---
extern char D_SoftwareVersion[15];
extern char D_Mfac[15];
extern char D_Model[15];

// --- Network Settings ---
extern char DEVICE_NAME[28];
extern char DEVICE_PASS[28];
extern char WIFI_SSID[30];
extern char WIFI_PASS[25];
extern char HOSTNAME[30];

// --- Static IP Settings ---
extern char DIVICE_IP[16];
extern char IP_ADDR[16];
extern char SUBNET_MASK[16];
extern char GATEWAY[16];

// --- MQTT Settings ---
extern char MQTT_SERVER[64];
extern char MQTT_USER[32];
extern char MQTT_PASS[32];
extern uint16_t MQTT_PORT;
extern WiFiClient espClient;
extern PubSubClient client;

// --- Auth & Status ---
extern char AUTH_USER[10];
extern char AUTH_PASS[10];

extern bool isWifiApMode;
extern String deviceName;

// --- Project Sub-Programs ---
#include "app_main.h"
#include "http_server.h"
#include "ha_integration.h"
#include "logger.h"
#include "network_manager.h"
#include "ota_update.h"
#include "pzem_monitor.h"
#include "storage_manager.h"
#include "time_sync.h"
#include "websocket_handler.h"
