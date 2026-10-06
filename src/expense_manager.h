#pragma once
#include "config.h"

struct ExpenseSettings {
    float unitCost1;
    float priceCost1;
    float unitCost2;
    float priceCost2;
    float priceCost3;
    float unitSolar;
    float ft;
    float serviceFee;
    float vatRate;
    char dailyResetTime[6];
    uint8_t monthlyResetDay;
};

void expenseInit();
bool expenseSaveSettings(const ExpenseSettings& settings);
const ExpenseSettings& getExpenseSettings();
float calculateProgressiveGridCost(float units);
float calculateEstimatedBill(float units);
void buildExpenseJson(String& out);
