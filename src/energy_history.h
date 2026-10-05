#pragma once
#include <Arduino.h>

void energyHistoryInit();
void energyHistoryUpdate(float totalEnergyKWh);
void energyHistoryRecordPower(float powerW);
void buildEnergyHistoryJson(String& out);
