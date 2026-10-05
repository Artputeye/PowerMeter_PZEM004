#include "websocket_handler.h"
#include "config.h"
#include "pzem_monitor.h"
#include <ArduinoJson.h>

void ws_init()
{
    ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client,
                  AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        if (type == WS_EVT_CONNECT)
            Serial.printf("[WS] Client #%u connected\n", client->id());
        else if (type == WS_EVT_DISCONNECT)
            Serial.printf("[WS] Client disconnected\n");
    });

    server.addHandler(&ws);
}

void notifyClients()
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

    String payload;
    serializeJson(doc, payload);
    ws.textAll(payload);
}

void wsProcess()
{
    static uint32_t lastSend = 0;
    if (millis() - lastSend < 500)
        return;

    lastSend = millis();
    notifyClients();
}
