#pragma once
#include "config.h"

void iotHAsetup();
void iotHAloop();
void reconnect();
void send_ha_discovery();
void publish_all_states();
void send_sensor_config(const char *state_key, const char *name, const char *unit,
                        const char *device_class, const char *icon, const char *category);
