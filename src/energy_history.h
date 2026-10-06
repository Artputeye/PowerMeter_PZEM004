#pragma once
#include "config.h"

void energyHistoryInit();
void energyHistoryUpdate(float totalEnergyKWh);
void energyHistoryRecordPower(float powerW);
void buildEnergyHistoryJson(String& out);
