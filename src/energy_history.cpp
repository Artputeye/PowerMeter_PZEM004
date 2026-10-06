#include "energy_history.h"
#include "config.h"
#include "expense_manager.h"

namespace {
constexpr size_t HOURLY_COUNT = 24;
constexpr size_t DAILY_COUNT = 30;
constexpr size_t MONTHLY_COUNT = 12;
constexpr size_t POWER_COUNT = 60;
constexpr unsigned long SAVE_INTERVAL_MS = 15UL * 60UL * 1000UL;
constexpr unsigned long POWER_SAMPLE_INTERVAL_MS = 60UL * 1000UL;
const char* HISTORY_FILE = "/energy_history.json";

float hourly[HOURLY_COUNT] = {};
float daily[DAILY_COUNT] = {};
float monthly[MONTHLY_COUNT] = {};
float powerHistory[POWER_COUNT] = {};

float lastEnergy = NAN;
unsigned long lastSaveMs = 0;
unsigned long lastPowerSampleMs = 0;
int64_t lastHourKey = 0;
int64_t lastDayKey = 0;
int64_t lastMonthKey = 0;
bool calendarReady = false;
portMUX_TYPE historyMux = portMUX_INITIALIZER_UNLOCKED;

void clearArray(float* values, size_t count) {
    for (size_t i = 0; i < count; ++i) values[i] = 0.0f;
}

void shiftAppend(float* values, size_t count, float value) {
    for (size_t i = 0; i + 1 < count; ++i) values[i] = values[i + 1];
    values[count - 1] = value;
}

void shiftBuckets(int64_t hourDelta, int64_t dayDelta, int64_t monthDelta) {
    if (hourDelta > 0) {
        if (hourDelta >= static_cast<int64_t>(HOURLY_COUNT)) clearArray(hourly, HOURLY_COUNT);
        else {
            const size_t shift = static_cast<size_t>(hourDelta);
            for (size_t i = 0; i < HOURLY_COUNT - shift; ++i) hourly[i] = hourly[i + shift];
            for (size_t i = HOURLY_COUNT - shift; i < HOURLY_COUNT; ++i) hourly[i] = 0.0f;
        }
    }

    if (dayDelta > 0) {
        if (dayDelta >= static_cast<int64_t>(DAILY_COUNT)) clearArray(daily, DAILY_COUNT);
        else {
            const size_t shift = static_cast<size_t>(dayDelta);
            for (size_t i = 0; i < DAILY_COUNT - shift; ++i) daily[i] = daily[i + shift];
            for (size_t i = DAILY_COUNT - shift; i < DAILY_COUNT; ++i) daily[i] = 0.0f;
        }
    }

    if (monthDelta > 0) {
        if (monthDelta >= static_cast<int64_t>(MONTHLY_COUNT)) clearArray(monthly, MONTHLY_COUNT);
        else {
            const size_t shift = static_cast<size_t>(monthDelta);
            for (size_t i = 0; i < MONTHLY_COUNT - shift; ++i) monthly[i] = monthly[i + shift];
            for (size_t i = MONTHLY_COUNT - shift; i < MONTHLY_COUNT; ++i) monthly[i] = 0.0f;
        }
    }
}

int getDailyResetMinute() {
    const ExpenseSettings& settings = getExpenseSettings();
    const char* value = settings.dailyResetTime;

    if (!value ||
        value[2] != ':' ||
        value[0] < '0' || value[0] > '2' ||
        value[1] < '0' || value[1] > '9' ||
        value[3] < '0' || value[3] > '5' ||
        value[4] < '0' || value[4] > '9') {
        return 0;
    }

    const int hour = (value[0] - '0') * 10 + (value[1] - '0');
    const int minute = (value[3] - '0') * 10 + (value[4] - '0');

    if (hour > 23) return 0;
    return hour * 60 + minute;
}

int getMonthlyResetDay() {
    const ExpenseSettings& settings = getExpenseSettings();
    return constrain(static_cast<int>(settings.monthlyResetDay), 1, 28);
}

bool getCalendarKeys(int64_t& hourKey, int64_t& dayKey, int64_t& monthKey) {
    const time_t now = time(nullptr);
    if (now < 1577836800) return false;

    struct tm local{};
    localtime_r(&now, &local);

    // Hourly history continues to follow the clock hour.
    hourKey = static_cast<int64_t>(now / 3600);

    const int resetMinute = getDailyResetMinute();
    const int minuteOfDay = local.tm_hour * 60 + local.tm_min;
    const bool beforeDailyReset = minuteOfDay < resetMinute;

    // Daily period: [reset time, next reset time).
    // If the current time is before today's reset, it belongs to yesterday's period.
    struct tm dailyDate = local;
    if (beforeDailyReset) {
        dailyDate.tm_mday -= 1;
    }
    dailyDate.tm_hour = 0;
    dailyDate.tm_min = 0;
    dailyDate.tm_sec = 0;
    const time_t dailyStart = mktime(&dailyDate);
    dayKey = static_cast<int64_t>(dailyStart / 86400);

    // Monthly period starts at MonthlyResetDay + DailyResetTime.
    // If we are before this month's reset boundary, use the previous month.
    const int resetDay = getMonthlyResetDay();
    const bool beforeMonthlyReset =
        (local.tm_mday < resetDay) ||
        (local.tm_mday == resetDay && minuteOfDay < resetMinute);

    struct tm monthDate = local;
    if (beforeMonthlyReset) {
        monthDate.tm_mon -= 1;
    }

    monthKey =
        static_cast<int64_t>(monthDate.tm_year + 1900) * 12 +
        static_cast<int64_t>(monthDate.tm_mon);

    return true;
}

void saveHistory() {
    JsonDocument doc;
    doc["version"] = 3;
    doc["hour_key"] = lastHourKey;
    doc["day_key"] = lastDayKey;
    doc["month_key"] = lastMonthKey;

    const ExpenseSettings& settings = getExpenseSettings();
    doc["daily_reset_time"] = settings.dailyResetTime;
    doc["monthly_reset_day"] = settings.monthlyResetDay;

    JsonArray h = doc["hourly"].to<JsonArray>();
    JsonArray d = doc["daily"].to<JsonArray>();
    JsonArray m = doc["monthly"].to<JsonArray>();

    portENTER_CRITICAL(&historyMux);
    for (size_t i = 0; i < HOURLY_COUNT; ++i) h.add(hourly[i]);
    for (size_t i = 0; i < DAILY_COUNT; ++i) d.add(daily[i]);
    for (size_t i = 0; i < MONTHLY_COUNT; ++i) m.add(monthly[i]);
    portEXIT_CRITICAL(&historyMux);

    File file = FILESYSTEM.open(HISTORY_FILE, FILE_WRITE);
    if (!file) return;
    serializeJson(doc, file);
    file.close();
}

void loadHistory() {
    if (!FILESYSTEM.exists(HISTORY_FILE)) return;

    File file = FILESYSTEM.open(HISTORY_FILE, FILE_READ);
    if (!file) return;

    JsonDocument doc;
    if (deserializeJson(doc, file)) {
        file.close();
        return;
    }
    file.close();

    portENTER_CRITICAL(&historyMux);
    clearArray(hourly, HOURLY_COUNT);
    clearArray(daily, DAILY_COUNT);
    clearArray(monthly, MONTHLY_COUNT);

    JsonArrayConst h = doc["hourly"].as<JsonArrayConst>();
    JsonArrayConst d = doc["daily"].as<JsonArrayConst>();
    JsonArrayConst m = doc["monthly"].as<JsonArrayConst>();

    for (size_t i = 0; i < min(h.size(), HOURLY_COUNT); ++i)
        hourly[i] = max(0.0f, h[i].as<float>());
    for (size_t i = 0; i < min(d.size(), DAILY_COUNT); ++i)
        daily[i] = max(0.0f, d[i].as<float>());
    for (size_t i = 0; i < min(m.size(), MONTHLY_COUNT); ++i)
        monthly[i] = max(0.0f, m[i].as<float>());

    lastHourKey = doc["hour_key"] | 0LL;
    lastDayKey = doc["day_key"] | 0LL;
    lastMonthKey = doc["month_key"] | 0LL;
    calendarReady = lastHourKey != 0 && lastDayKey != 0 && lastMonthKey != 0;
    portEXIT_CRITICAL(&historyMux);

    // If the reset schedule was changed since the history file was written,
    // keep the existing accumulated history but re-anchor it to the new period.
    const char* savedResetTime = doc["daily_reset_time"] | "";
    const int savedResetDay = doc["monthly_reset_day"] | 0;
    const ExpenseSettings& settings = getExpenseSettings();

    if (strcmp(savedResetTime, settings.dailyResetTime) != 0 ||
        savedResetDay != static_cast<int>(settings.monthlyResetDay)) {
        calendarReady = false;
    }
}
}

void energyHistoryInit() {
    loadHistory();
    lastSaveMs = millis();
    lastPowerSampleMs = millis();
    lastEnergy = NAN;
}

void energyHistoryUpdate(float totalEnergyKWh) {
    if (!isfinite(totalEnergyKWh) || totalEnergyKWh < 0.0f) return;

    int64_t hourKey, dayKey, monthKey;
    if (!getCalendarKeys(hourKey, dayKey, monthKey)) return;

    float delta = 0.0f;
    if (isfinite(lastEnergy) && totalEnergyKWh >= lastEnergy) {
        delta = totalEnergyKWh - lastEnergy;
    }
    lastEnergy = totalEnergyKWh;

    bool saveNow = false;

    portENTER_CRITICAL(&historyMux);

    if (!calendarReady) {
        lastHourKey = hourKey;
        lastDayKey = dayKey;
        lastMonthKey = monthKey;
        calendarReady = true;
    } else {
        const int64_t hourDelta = hourKey - lastHourKey;
        const int64_t dayDelta = dayKey - lastDayKey;
        const int64_t monthDelta = monthKey - lastMonthKey;

        if (hourDelta > 0 || dayDelta > 0 || monthDelta > 0) {
            shiftBuckets(hourDelta, dayDelta, monthDelta);
            lastHourKey = hourKey;
            lastDayKey = dayKey;
            lastMonthKey = monthKey;
        }
    }

    hourly[HOURLY_COUNT - 1] += delta;
    daily[DAILY_COUNT - 1] += delta;
    monthly[MONTHLY_COUNT - 1] += delta;

    const unsigned long nowMs = millis();
    if (nowMs - lastSaveMs >= SAVE_INTERVAL_MS) {
        lastSaveMs = nowMs;
        saveNow = true;
    }

    portEXIT_CRITICAL(&historyMux);

    if (saveNow) saveHistory();
}

void energyHistoryRecordPower(float powerW) {
    if (!isfinite(powerW)) return;

    const unsigned long nowMs = millis();
    if (nowMs - lastPowerSampleMs < POWER_SAMPLE_INTERVAL_MS) return;
    lastPowerSampleMs = nowMs;

    portENTER_CRITICAL(&historyMux);
    shiftAppend(powerHistory, POWER_COUNT, max(0.0f, powerW));
    portEXIT_CRITICAL(&historyMux);
}

void buildEnergyHistoryJson(String& out) {
    JsonDocument doc;
    JsonArray h = doc["hourly"].to<JsonArray>();
    JsonArray d = doc["daily"].to<JsonArray>();
    JsonArray m = doc["monthly"].to<JsonArray>();
    JsonArray p = doc["power"].to<JsonArray>();

    portENTER_CRITICAL(&historyMux);
    for (size_t i = 0; i < HOURLY_COUNT; ++i) h.add(hourly[i]);
    for (size_t i = 0; i < DAILY_COUNT; ++i) d.add(daily[i]);
    for (size_t i = 0; i < MONTHLY_COUNT; ++i) m.add(monthly[i]);
    for (size_t i = 0; i < POWER_COUNT; ++i) p.add(powerHistory[i]);
    portEXIT_CRITICAL(&historyMux);

    serializeJson(doc, out);
}
