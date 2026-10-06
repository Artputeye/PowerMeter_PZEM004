#pragma once

#include <Arduino.h>
#include <esp_system.h>
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

#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern PZEM004Tv30 pzem;

extern char D_SoftwareVersion[15];
extern char D_Mfac[15];
extern char D_Model[15];

extern char DEVICE_NAME[28];
extern char DEVICE_PASS[28];
extern char WIFI_SSID[30];
extern char WIFI_PASS[25];
extern char HOSTNAME[30];

extern char DIVICE_IP[16];
extern char IP_ADDR[16];
extern char SUBNET_MASK[16];
extern char GATEWAY[16];

extern char MQTT_SERVER[64];
extern char MQTT_USER[32];
extern char MQTT_PASS[32];
extern uint16_t MQTT_PORT;
extern WiFiClient espClient;
extern PubSubClient client;

extern char AUTH_USER[10];
extern char AUTH_PASS[10];

extern bool isWifiApMode;
extern bool isIpConfigStatic;
extern String deviceName;
