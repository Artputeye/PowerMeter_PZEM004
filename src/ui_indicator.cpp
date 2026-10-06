#include "ui_indicator.h"

volatile LedMode ledMode = LED_DISCONNECTED;

void ledIndicator(unsigned long onTime, unsigned long offTime)
{
    static bool ledState = false;
    static unsigned long previousMillis = 0;

    const unsigned long currentMillis = millis();
    const unsigned long interval = ledState ? onTime : offTime;

    if (currentMillis - previousMillis < interval)
        return;

    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(STATUS_LED, ledState ? HIGH : LOW);
}

void ledPatternSelect()
{
    switch (ledMode)
    {
        case LED_OFF:
            digitalWrite(STATUS_LED, LOW);
            break;

        case LED_ON:
            digitalWrite(STATUS_LED, HIGH);
            break;

        case LED_CONNECTED:
            ledIndicator(100, 2000);
            break;

        case LED_DISCONNECTED:
            ledIndicator(100, 100);
            break;

        case LED_AP_MODE:
            ledIndicator(1000, 1000);
            break;

        case LED_MQTT_FAIL:
            ledIndicator(500, 500);
            break;

        case LED_OTA_RUNNING:
            ledIndicator(200, 200);
            break;

        case LED_FAULT:
            ledIndicator(500, 500);
            break;

        case LED_BUSY:
            ledIndicator(200, 200);
            break;
    }
}
