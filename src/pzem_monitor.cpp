#include "pzem_monitor.h"
#include "config.h"
#include "energy_history.h"

PzemData pzemData;
void pzem_init()
{
    Serial2.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
    Serial.println(F("[PZEM] Initialized on Serial2"));
}

void pzem_update()
{
    PzemData next;
    next.voltage = pzem.voltage();
    next.current = pzem.current();
    next.power = pzem.power();
    next.energy = pzem.energy();
    next.frequency = pzem.frequency();
    next.pf = pzem.pf();

    next.valid = !isnan(next.voltage) || !isnan(next.current) || !isnan(next.power);
    pzemData = next;

    if (next.valid)
    {
        energyHistoryUpdate(next.energy);
        energyHistoryRecordPower(next.power);
    }
}

const PzemData& getPzemData()
{
    return pzemData;
}
