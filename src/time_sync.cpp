#include "time_sync.h"
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

void NTPbegin()
{
    configTime(7 * 3600, 0, "pool.ntp.org");
    Serial.println(F("[NTP] Time sync started"));
}
