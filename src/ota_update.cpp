#include "ota_update.h"
#include "config.h"

void ota_init()
{
    server.on("/update", HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            bool ok = !Update.hasError();
            request->send(ok ? 200 : 500, "text/plain", ok ? "OK" : "FAIL");
        },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            if (!index)
                Update.begin(UPDATE_SIZE_UNKNOWN);

            Update.write(data, len);

            if (final)
                Update.end(true);
        });
}
