#include "ota_update.h"
#include "config.h"
#include "ui_indicator.h"

namespace
{
File fsUploadFile;
}

void ota_init()
{
    handleFirmwareUpload();
    handleFileSystemUpload();
}

void handleFirmwareUpload()
{
    server.on("/otafirmware", HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            const bool success = !Update.hasError();
            if (success)
            {
                request->send(200, "text/plain", "OK");
                vTaskDelay(pdMS_TO_TICKS(1000));
                ESP.restart();
            }
            else
                request->send(500, "text/plain", "Update Failed");
        },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            if (index == 0)
            {
                ledMode = LED_OTA_RUNNING;
                Serial.printf("[OTA] Start: %s\n", filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH))
                    Update.printError(Serial);
            }
            if (!Update.hasError())
            {
                if (Update.write(data, len) != len)
                    Update.printError(Serial);
            }
            if (final)
            {
                if (Update.end(true))
                    Serial.printf("[OTA] Success: %u bytes\n", index + len);
                else
                    Update.printError(Serial);
            }
        });
}

void handleFileSystemUpload()
{
    server.on("/otalittlefs", HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            request->send(200, "text/plain", "OK");
        },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            if (!filename.startsWith("/"))
                filename = "/" + filename;

            if (index == 0)
            {
                const int lastSlash = filename.lastIndexOf('/');
                if (lastSlash > 0)
                {
                    const String dirPath = filename.substring(0, lastSlash);
                    if (!FILESYSTEM.exists(dirPath))
                        FILESYSTEM.mkdir(dirPath);
                }
                ledMode = LED_OTA_RUNNING;
                Serial.printf("[FS] Uploading: %s\n", filename.c_str());
                fsUploadFile = FILESYSTEM.open(filename, FILE_WRITE);
            }

            if (fsUploadFile)
                fsUploadFile.write(data, len);

            if (final)
            {
                if (fsUploadFile)
                    fsUploadFile.close();
                Serial.printf("[FS] Upload Complete: %u bytes\n", index + len);
            }
        });
}
