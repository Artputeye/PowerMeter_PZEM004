#include "app_main.h"
#include "config.h"
#include "energy_history.h"

void app_setup()
{
    pzem_init();
    storage_init();
    energyHistoryInit();
    setupWebServer();
}

void app_loop()
{
    pzem_update();
}
