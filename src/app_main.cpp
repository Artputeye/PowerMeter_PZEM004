#include "app_main.h"
#include "config.h"
#include "pzem_monitor.h"
#include "http_server.h"
#include "energy_history.h"
#include "storage_manager.h"

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
