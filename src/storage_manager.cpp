#include "storage_manager.h"
#include "config.h"
#include "expense_manager.h"

void storage_init()
{
    Serial.println(F("[Storage] LittleFS ready"));
    expenseInit();
}
