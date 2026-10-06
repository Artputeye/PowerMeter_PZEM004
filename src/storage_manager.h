#pragma once

#include "config.h"
#include <ArduinoJson.h>

bool storage_exists(const char* path);
bool storage_save_json(const char* path, const JsonDocument& doc);
bool storage_load_json(const char* path, JsonDocument& doc);
bool storage_remove(const char* path);

void setupStorageManagement();
void handleFileList();
void handleFileDelete();

void storage_init();
