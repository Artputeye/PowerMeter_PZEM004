#include "logger.h"

namespace {
constexpr const char* LOG_FILE_PATH = "/system_log.txt";
constexpr size_t MAX_LOG_SIZE_BYTES = 10 * 1024;
}

void writeLog(const String& level, const String& message)
{
    if (FILESYSTEM.exists(LOG_FILE_PATH)) {
        File checkFile = FILESYSTEM.open(LOG_FILE_PATH, FILE_READ);
        if (checkFile) {
            const size_t size = checkFile.size();
            checkFile.close();
            if (size >= MAX_LOG_SIZE_BYTES) FILESYSTEM.remove(LOG_FILE_PATH);
        }
    }

    File logFile = FILESYSTEM.open(LOG_FILE_PATH, FILE_APPEND);
    if (!logFile) return;
    logFile.printf("[%lus] [%s] %s\
", millis() / 1000UL, level.c_str(), message.c_str());
    logFile.close();
}

void logger_init() {}

void displayLogs()
{
    if (!FILESYSTEM.exists(LOG_FILE_PATH)) { Serial.println(F("[Logger] No log history")); return; }
    File logFile = FILESYSTEM.open(LOG_FILE_PATH, FILE_READ);
    if (!logFile) { Serial.println(F("[Logger] Failed to open log history")); return; }
    while (logFile.available()) Serial.write(logFile.read());
    logFile.close();
}

void checkAndLogResetReason()
{
    const esp_reset_reason_t reason = esp_reset_reason();
    if (reason == ESP_RST_PANIC || reason == ESP_RST_INT_WDT || reason == ESP_RST_TASK_WDT ||
        reason == ESP_RST_WDT || reason == ESP_RST_BROWNOUT) {
        writeLog("CRITICAL", "Abnormal reset reason=" + String(static_cast<int>(reason)) +
                 " FreeHeap=" + String(ESP.getFreeHeap()));
    }
}

void clearLogHistory()
{
    if (FILESYSTEM.exists(LOG_FILE_PATH)) FILESYSTEM.remove(LOG_FILE_PATH);
}
