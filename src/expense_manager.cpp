#include "expense_manager.h"
#include "config.h"
#include "energy_history.h"
#include <ArduinoJson.h>

namespace {
ExpenseSettings settings = {200.0f, 3.0000f, 400.0f, 4.1584f, 4.3583f, 450.0f, 0.3972f, 38.22f, 7.0f};

bool loadSettings() {
    if (!FILESYSTEM.exists("/expense.json")) return false;
    File file = FILESYSTEM.open("/expense.json", FILE_READ);
    if (!file) return false;

    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) return false;

    settings.unitCost1 = doc["UnitCost1"] | 200.0f;
    settings.priceCost1 = doc["PriceCost1"] | 3.0000f;
    settings.unitCost2 = doc["UnitCost2"] | 400.0f;
    settings.priceCost2 = doc["PriceCost2"] | 4.1584f;
    settings.priceCost3 = doc["PriceCost3"] | 4.3583f;
    settings.unitSolar = doc["UnitSolar"] | 450.0f;
    settings.ft = doc["ft"] | 0.3972f;
    settings.serviceFee = doc["ServiceFee"] | 38.22f;
    settings.vatRate = doc["VatRate"] | 7.0f;

    if (!isfinite(settings.unitCost1) || settings.unitCost1 < 0) settings.unitCost1 = 200.0f;
    if (!isfinite(settings.priceCost1) || settings.priceCost1 < 0) settings.priceCost1 = 3.0000f;
    if (!isfinite(settings.unitCost2) || settings.unitCost2 < settings.unitCost1) settings.unitCost2 = 400.0f;
    if (!isfinite(settings.priceCost2) || settings.priceCost2 < 0) settings.priceCost2 = 4.1584f;
    if (!isfinite(settings.priceCost3) || settings.priceCost3 < 0) settings.priceCost3 = 4.3583f;
    if (!isfinite(settings.unitSolar) || settings.unitSolar < 0) settings.unitSolar = 450.0f;
    if (!isfinite(settings.ft)) settings.ft = 0.3972f;
    if (!isfinite(settings.serviceFee) || settings.serviceFee < 0) settings.serviceFee = 38.22f;
    if (!isfinite(settings.vatRate) || settings.vatRate < 0 || settings.vatRate > 100) settings.vatRate = 7.0f;
    return true;
}
}

void expenseInit() {
    Serial.println(loadSettings() ? F("[Expense] Settings loaded") : F("[Expense] Using default settings"));
}

bool expenseSaveSettings(const ExpenseSettings& incoming) {
    settings = incoming;

    JsonDocument doc;
    doc["UnitCost1"] = settings.unitCost1;
    doc["PriceCost1"] = settings.priceCost1;
    doc["UnitCost2"] = settings.unitCost2;
    doc["PriceCost2"] = settings.priceCost2;
    doc["PriceCost3"] = settings.priceCost3;
    doc["UnitSolar"] = settings.unitSolar;
    doc["ft"] = settings.ft;
    doc["ServiceFee"] = settings.serviceFee;
    doc["VatRate"] = settings.vatRate;

    File file = FILESYSTEM.open("/expense.json", FILE_WRITE);
    if (!file) return false;
    const size_t written = serializeJson(doc, file);
    file.close();
    return written > 0;
}

const ExpenseSettings& getExpenseSettings() {
    return settings;
}

float calculateProgressiveGridCost(float units) {
    if (!isfinite(units) || units <= 0) return 0.0f;

    const float tier1Limit = max(0.0f, settings.unitCost1);
    const float tier2Limit = max(tier1Limit, settings.unitCost2);

    float cost = min(units, tier1Limit) * settings.priceCost1;
    if (units > tier1Limit) {
        cost += min(units - tier1Limit, tier2Limit - tier1Limit) * settings.priceCost2;
    }
    if (units > tier2Limit) {
        cost += (units - tier2Limit) * settings.priceCost3;
    }
    return max(0.0f, cost);
}

float calculateEstimatedBill(float units) {
    units = max(0.0f, units);
    const float subtotal = calculateProgressiveGridCost(units) + units * settings.ft + settings.serviceFee;
    return max(0.0f, subtotal) * (1.0f + settings.vatRate / 100.0f);
}

void buildExpenseJson(String& out) {
    String history;
    buildEnergyHistoryJson(history);

    JsonDocument historyDoc;
    deserializeJson(historyDoc, history);

    const float monthlyEnergy = max(0.0f, historyDoc["monthly"][11] | 0.0f);
    const float todayEnergy = max(0.0f, historyDoc["daily"][29] | 0.0f);

    JsonDocument doc;
    doc["UnitCost1"] = settings.unitCost1;
    doc["PriceCost1"] = settings.priceCost1;
    doc["UnitCost2"] = settings.unitCost2;
    doc["PriceCost2"] = settings.priceCost2;
    doc["PriceCost3"] = settings.priceCost3;
    doc["UnitSolar"] = settings.unitSolar;
    doc["ft"] = settings.ft;
    doc["ServiceFee"] = settings.serviceFee;
    doc["VatRate"] = settings.vatRate;
    doc["energyToday"] = todayEnergy;
    doc["energyMonth"] = monthlyEnergy;
    doc["estimatedBill"] = calculateEstimatedBill(monthlyEnergy);
    serializeJson(doc, out);
}
