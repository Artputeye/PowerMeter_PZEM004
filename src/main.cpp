#include "config.h"

void TaskMain(void *pvParameters);
void TaskNetwork(void *pvParameters);

void setup()
{
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, HIGH);

    Serial.begin(115200);
    delay(500);
    Serial.println(F("\n[System] Booting PowerMeter_PZEM004"));

    if (!FILESYSTEM.begin(true))
        Serial.println(F("[System] LittleFS mount failed"));
    else
        Serial.println(F("[System] LittleFS mounted"));

    logger_init();
    network_setup();
    iotHAsetup();

    if (WiFi.status() == WL_CONNECTED)
        NTPbegin();

    app_setup();

    xTaskCreatePinnedToCore(TaskMain, "Main", 8192, nullptr, 2, nullptr, 0);
    xTaskCreatePinnedToCore(TaskNetwork, "Network", 8192, nullptr, 1, nullptr, 1);

    Serial.println(F("[System] Setup Complete"));
}

void loop()
{
    vTaskDelete(nullptr);
}

void TaskMain(void *pvParameters)
{
    for (;;)
    {
        app_loop();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void TaskNetwork(void *pvParameters)
{
    for (;;)
    {
        keepWiFiAlive();
        wsProcess();
        iotHAloop();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
