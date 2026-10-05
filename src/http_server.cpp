#include "http_server.h"
#include "config.h"
#include "pzem_monitor.h"
#include "websocket_handler.h"
#include "ota_update.h"
#include "energy_history.h"
#include "expense_manager.h"
#include <ArduinoJson.h>

String getContentType(const String& filename)
{
    if (filename.endsWith(".html")) return "text/html";
    if (filename.endsWith(".css")) return "text/css";
    if (filename.endsWith(".js")) return "application/javascript";
    if (filename.endsWith(".json")) return "application/json";
    if (filename.endsWith(".png")) return "image/png";
    return "text/plain";
}

void setupWebServer()
{
    ws_init();

    server.on("/api/pzem", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        JsonDocument doc;
        const auto& d = getPzemData();
        doc["voltage"] = d.voltage;
        doc["current"] = d.current;
        doc["power"] = d.power;
        doc["energy"] = d.energy;
        doc["frequency"] = d.frequency;
        doc["pf"] = d.pf;
        doc["valid"] = d.valid;
        String out;
        serializeJson(doc, out);
        request->send(200, "application/json", out);
    });

    server.on("/api/history", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String out;
        buildEnergyHistoryJson(out);
        request->send(200, "application/json", out);
    });

    server.on("/api/expense", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String out;
        buildExpenseJson(out);
        request->send(200, "application/json", out);
    });

    server.on("/api/expense", HTTP_POST, [](AsyncWebServerRequest *request) {}, nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            static String body;
            if (index == 0) body = "";
            body.reserve(total);
            body.concat(reinterpret_cast<const char*>(data), len);

            if (index + len != total) return;

            JsonDocument doc;
            if (deserializeJson(doc, body))
            {
                request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
                return;
            }

            ExpenseSettings settings = getExpenseSettings();
            settings.unitCost1 = doc["UnitCost1"] | settings.unitCost1;
            settings.priceCost1 = doc["PriceCost1"] | settings.priceCost1;
            settings.unitCost2 = doc["UnitCost2"] | settings.unitCost2;
            settings.priceCost2 = doc["PriceCost2"] | settings.priceCost2;
            settings.priceCost3 = doc["PriceCost3"] | settings.priceCost3;
            settings.unitSolar = doc["UnitSolar"] | settings.unitSolar;
            settings.ft = doc["ft"] | settings.ft;
            settings.serviceFee = doc["ServiceFee"] | settings.serviceFee;
            settings.vatRate = doc["VatRate"] | settings.vatRate;

            if (!isfinite(settings.unitCost1) || settings.unitCost1 < 0 ||
                !isfinite(settings.priceCost1) || settings.priceCost1 < 0 ||
                !isfinite(settings.unitCost2) || settings.unitCost2 < settings.unitCost1 ||
                !isfinite(settings.priceCost2) || settings.priceCost2 < 0 ||
                !isfinite(settings.priceCost3) || settings.priceCost3 < 0 ||
                !isfinite(settings.unitSolar) || settings.unitSolar < 0 ||
                !isfinite(settings.ft) ||
                !isfinite(settings.serviceFee) || settings.serviceFee < 0 ||
                !isfinite(settings.vatRate) || settings.vatRate < 0 || settings.vatRate > 100)
            {
                request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid expense values\"}");
                return;
            }

            if (!expenseSaveSettings(settings))
            {
                request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save expense settings\"}");
                return;
            }

            request->send(200, "application/json", "{\"status\":\"success\"}");
        });

    server.on("/api/network", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        JsonDocument doc;
        bool connected = WiFi.status() == WL_CONNECTED;
        doc["connected"] = connected;
        doc["ssid"] = connected ? WiFi.SSID() : "";
        doc["ip"] = connected ? WiFi.localIP().toString() : "";
        doc["gateway"] = connected ? WiFi.gatewayIP().toString() : "";
        doc["rssi"] = connected ? WiFi.RSSI() : 0;
        doc["mac"] = WiFi.macAddress();
        doc["hostname"] = "powermeter";
        String out;
        serializeJson(doc, out);
        request->send(200, "application/json", out);
    });

    server.serveStatic("/", FILESYSTEM, "/").setDefaultFile("index.html");
    ota_init();
    server.begin();
    Serial.println(F("[HTTP] Server started"));
}
