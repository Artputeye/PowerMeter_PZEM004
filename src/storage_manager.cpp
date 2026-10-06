#include "storage_manager.h"
#include "config.h"
#include "logger.h"
#include "expense_manager.h"

namespace {
bool validPath(const char* path)
{
    return path != nullptr && path[0] == '/';
}
}

bool storage_exists(const char* path)
{
    return validPath(path) && FILESYSTEM.exists(path);
}

bool storage_save_json(const char* path, const JsonDocument& doc)
{
    if (!validPath(path))
        return false;

    File file = FILESYSTEM.open(path, FILE_WRITE);
    if (!file)
        return false;

    const size_t written = serializeJson(doc, file);
    file.close();

    return written > 0;
}

bool storage_load_json(const char* path, JsonDocument& doc)
{
    if (!validPath(path) || !FILESYSTEM.exists(path))
        return false;

    File file = FILESYSTEM.open(path, FILE_READ);
    if (!file)
        return false;

    const DeserializationError error = deserializeJson(doc, file);
    file.close();

    return !error;
}

bool storage_remove(const char* path)
{
    if (!validPath(path))
        return false;

    if (!FILESYSTEM.exists(path))
        return true;

    return FILESYSTEM.remove(path);
}

void storage_init()
{
    Serial.println(F("[Storage] LittleFS ready"));
    expenseInit();
}

void handleFileList()
{
    server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        if (!request->hasParam("dir"))
        {
            request->send(400, "text/plain", "Missing 'dir' param");
            return;
        }

        const String path = request->getParam("dir")->value();
        File root = FILESYSTEM.open(path);
        String output = "[";

        if (root && root.isDirectory())
        {
            File file = root.openNextFile();
            while (file)
            {
                if (output != "[")
                    output += ",";

                output += "{\"type\":\"";
                output += (file.isDirectory() ? "dir" : "file");
                output += "\",\"name\":\"";
                output += String(file.path());
                output += "\",\"size\":";
                output += String(file.size());
                output += "}";

                file = root.openNextFile();
            }
        }

        output += "]";
        request->send(200, "application/json", output);
    });
}

void handleFileDelete()
{
    server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        if (!request->hasParam("file"))
        {
            request->send(400, "text/plain", "Missing 'file' param");
            return;
        }

        const String path = request->getParam("file")->value();

        if (path == "/" || !FILESYSTEM.exists(path))
        {
            request->send(404, "text/plain", "Invalid Path");
            return;
        }

        if (FILESYSTEM.remove(path))
        {
            Serial.printf("[FS] Deleted: %s\n", path.c_str());
            request->send(200, "text/plain", "Deleted");
        }
        else
            request->send(500, "text/plain", "Delete Failed");
    });
}

void setupStorageManagement()
{
    handleFileList();
    handleFileDelete();
}
