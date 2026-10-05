#include "network_manager.h"
#include "config.h"

void network_setup()
{
    WiFi.mode(WIFI_STA);

    WiFiManager wm;
    wm.setConfigPortalTimeout(180);

    String apName = deviceName + "-Setup";
    if (!wm.autoConnect(apName.c_str()))
    {
        Serial.println(F("[WiFi] Config portal timeout"));
        isWifiApMode = true;
        return;
    }

    isWifiApMode = false;
    Serial.printf("[WiFi] Connected: %s / IP=%s\n",
                  WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

    if (MDNS.begin("powermeter"))
        Serial.println(F("[mDNS] powermeter.local"));
}

void keepWiFiAlive()
{
    if (WiFi.status() == WL_CONNECTED)
        return;

    static uint32_t lastAttempt = 0;
    if (millis() - lastAttempt < 10000)
        return;

    lastAttempt = millis();
    WiFi.reconnect();
}

void apModeCheck()
{
    if (digitalRead(AP_PIN) == LOW)
        WiFi.disconnect(true);
}


