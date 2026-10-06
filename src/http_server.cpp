#include "http_server.h"
#include "config.h"\n#include "logger.h"
#include "pzem_monitor.h"
#include "websocket_handler.h"
#include "ota_update.h"
#include "energy_history.h"
#include "expense_manager.h"
#include "storage_manager.h"

namespace
{
void routeSettingAPI(const String& filename, const String& mode)
{
    String fsPath = filename;
    while (fsPath.startsWith("/"))
        fsPath = fsPath.substring(1);
    fsPath = "/" + fsPath;

    if (!storage_exists(fsPath.c_str()))
    {
        JsonDocument empty;
        storage_save_json(fsPath.c_str(), empty);
        Serial.printf("[LittleFS] Created %s\n", fsPath.c_str());
    }

    if (mode == "r" || mode == "rw")
    {
        server.on(fsPath.c_str(), HTTP_GET, [fsPath](AsyncWebServerRequest *request)
        {
            JsonDocument doc;
            if (!storage_load_json(fsPath.c_str(), doc))
            {
                request->send(200, "application/json", "{}");
                return;
            }

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        });
    }

    if (mode == "w" || mode == "rw")
    {
        server.on(fsPath.c_str(), HTTP_POST, [](AsyncWebServerRequest *request) {}, nullptr,
            [fsPath](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                static String jsonBuffer;

                if (index == 0)
                {
                    jsonBuffer = "";
                    jsonBuffer.reserve(total);
                }

                jsonBuffer.concat(reinterpret_cast<const char*>(data), len);

                if (index + len != total)
                    return;

                JsonDocument doc;
                if (deserializeJson(doc, jsonBuffer))
                {
                    request->send(400, "application/json",
                                  "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
                    return;
                }

                if (!storage_save_json(fsPath.c_str(), doc))
                {
                    request->send(500, "application/json",
                                  "{\"status\":\"error\",\"message\":\"Failed to save configuration\"}");
                    return;
                }

                Serial.printf("[HTTP] POST %s : Saved successfully\n", fsPath.c_str());
                request->send(200, "application/json", "{\"status\":\"success\"}");
            });
    }
}

void setupRouteAPIs()
{
    // Same file-backed API pattern as Hybrid Inverter.
    // network.js uses this exact endpoint.
    routeSettingAPI("/networkconfig.json", "rw");
}

void staticRoot()
{
    server.serveStatic("/", FILESYSTEM, "/")
        .setDefaultFile("index.html")
        .setCacheControl("max-age=86400");

    const char* pages[] = {
        "/network", "/dashboard", "/setting", "/ota", "/console"
    };

    for (const char* page : pages)
    {
        server.on(page, HTTP_GET, [page](AsyncWebServerRequest *request)
        {
            String filepath = String(page) + ".html";
            if (FILESYSTEM.exists(filepath))
                request->send(FILESYSTEM, filepath, "text/html");
            else
                request->send(404, "text/plain", "Page not found");
        });
    }
}

void terminalSetting()
{
    server.on("/terminalSet", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        String message;
        if (request->hasParam("plain", true))
            message = request->getParam("plain", true)->value();

        if (message == "espreset")
        {
            request->send(200, "text/plain", "POST: espreset");
            delay(100);
            ESP.restart();
            return;
        }

        request->send(200, "text/plain", "POST: " + message);
    });
}

void notfoundRoot()
{
    server.onNotFound([](AsyncWebServerRequest *request)
    {
        const String path = request->url();

        if (FILESYSTEM.exists(path))
        {
            request->send(FILESYSTEM, path, getContentType(path));
            return;
        }

        Serial.println("[HTTP] 404 Not Found: " + path);
        request->send(404, "text/plain", "File Not Found");
    });
}
}

String getContentType(const String& filename)
{
    if (filename.endsWith(".html")) return "text/html";
    if (filename.endsWith(".css")) return "text/css";
    if (filename.endsWith(".js")) return "application/javascript";
    if (filename.endsWith(".json")) return "application/json";
    if (filename.endsWith(".png")) return "image/png";
    if (filename.endsWith(".jpg")) return "image/jpeg";
    if (filename.endsWith(".jpeg")) return "image/jpeg";
    if (filename.endsWith(".ico")) return "image/x-icon";
    if (filename.endsWith(".svg")) return "image/svg+xml";
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
            if (index == 0)
            {
                body = "";
                body.reserve(total);
            }

            body.concat(reinterpret_cast<const char*>(data), len);
            if (index + len != total) return;

            JsonDocument doc;
            if (deserializeJson(doc, body))
            {
                request->send(400, "application/json",
                              "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
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
                request->send(400, "application/json",
                              "{\"status\":\"error\",\"message\":\"Invalid expense values\"}");
                return;
            }

            if (!expenseSaveSettings(settings))
            {
                request->send(500, "application/json",
                              "{\"status\":\"error\",\"message\":\"Failed to save expense settings\"}");
                return;
            }

            request->send(200, "application/json", "{\"status\":\"success\"}");
        });

    server.on("/api/network", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        JsonDocument doc;
        const bool connected = WiFi.status() == WL_CONNECTED;
        doc["connected"] = connected;
        doc["ssid"] = connected ? WiFi.SSID() : "";
        doc["ip"] = connected ? WiFi.localIP().toString() : "";
        doc["gateway"] = connected ? WiFi.gatewayIP().toString() : "";
        doc["rssi"] = connected ? WiFi.RSSI() : 0;
        doc["mac"] = WiFi.macAddress();
        doc["hostname"] = HOSTNAME;

        String out;
        serializeJson(doc, out);
        request->send(200, "application/json", out);
    });

    setupRouteAPIs();
    staticRoot();
    terminalSetting();
    notfoundRoot();

    setupStorageManagement();
    ota_init();
    server.begin();
    Serial.println(F("[HTTP] Server started"));
}
