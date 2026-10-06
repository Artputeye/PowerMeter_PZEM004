#pragma once
#include "config.h"

void logger_init();
void writeLog(const String& level, const String& message);
void displayLogs();
void checkAndLogResetReason();
void clearLogHistory();
