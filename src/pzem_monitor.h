#pragma once
#include "config.h"

struct PzemData {
    float voltage = NAN;
    float current = NAN;
    float power = NAN;
    float energy = NAN;
    float frequency = NAN;
    float pf = NAN;
    bool valid = false;
};

extern PzemData pzemData;

void pzem_init();
void pzem_update();
const PzemData& getPzemData();
