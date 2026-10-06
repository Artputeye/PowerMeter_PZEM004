#include "time_sync.h"
#include "config.h"\n#include "logger.h"

void NTPbegin()
{
    configTime(7 * 3600, 0, "pool.ntp.org");
    Serial.println(F("[NTP] Time sync started"));
}
