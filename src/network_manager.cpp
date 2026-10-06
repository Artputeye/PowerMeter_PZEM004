#include "network_manager.h"
#include "logger.h"
#include "storage_manager.h"

namespace
{
constexpr uint32_t RECONNECT_INTERVAL_MS = 10000;
constexpr uint32_t AP_CHANNEL = 6;
constexpr uint8_t AP_MAX_CLIENTS = 4;

uint32_t lastReconnectAttempt = 0;

bool loadString(JsonVariantConst value, char* destination, size_t destinationSize)
{
    if (value.isNull() || destination == nullptr || destinationSize == 0)
        return false;

    const char* valueText = value.as<const char*>();
    if (valueText == nullptr)
        return false;

    strlcpy(destination, valueText, destinationSize);
    return true;
}

int loadInt(JsonVariantConst value, int fallback)
{
    if (value.is<int>())
        return value.as<int>();

    if (value.is<const char*>())
    {
        const char* text = value.as<const char*>();
        if (text != nullptr && *text != '\0')
            return atoi(text);
    }

    return fallback;
}

void saveDefaultNetworkConfig()
{
    JsonDocument doc;
    doc["wifi_mode"] = "0";
    doc["ip_config"] = "0";
    doc["device_name"] = DEVICE_NAME;
    doc["hostname"] = HOSTNAME;
    doc["wifi_ssid"] = WIFI_SSID;
    doc["wifi_pass"] = WIFI_PASS;
    doc["ip_address"] = IP_ADDR;
    doc["subnet_mask"] = SUBNET_MASK;
    doc["default_gateway"] = GATEWAY;
    doc["mqtt_server"] = MQTT_SERVER;
    doc["mqtt_user"] = MQTT_USER;
    doc["mqtt_pass"] = MQTT_PASS;
    doc["mqtt_port"] = MQTT_PORT;
    storage_save_json("/networkconfig.json", doc);
}

bool loadNetworkConfig()
{
    constexpr const char* PATH = "/networkconfig.json";

    if (!storage_exists(PATH))
    {
        saveDefaultNetworkConfig();
        writeLog("WARN", "Network config not found; created default configuration");
        return false;
    }

    JsonDocument doc;
    if (!storage_load_json(PATH, doc))
    {
        saveDefaultNetworkConfig();
        writeLog("ERROR", "Invalid /networkconfig.json; restored defaults");
        return false;
    }

    // network.js stores these two values as strings ("0"/"1").
    isWifiApMode = loadInt(doc["wifi_mode"], isWifiApMode ? 1 : 0) != 0;
    isIpConfigStatic = loadInt(doc["ip_config"], isIpConfigStatic ? 1 : 0) != 0;

    loadString(doc["wifi_ssid"], WIFI_SSID, sizeof(WIFI_SSID));
    loadString(doc["wifi_pass"], WIFI_PASS, sizeof(WIFI_PASS));
    loadString(doc["device_name"], DEVICE_NAME, sizeof(DEVICE_NAME));
    loadString(doc["hostname"], HOSTNAME, sizeof(HOSTNAME));

    loadString(doc["ip_address"], IP_ADDR, sizeof(IP_ADDR));
    loadString(doc["subnet_mask"], SUBNET_MASK, sizeof(SUBNET_MASK));

    // network.html uses id="default_gateway"; accept "gateway" too for compatibility.
    if (!loadString(doc["gateway"], GATEWAY, sizeof(GATEWAY)))
        loadString(doc["default_gateway"], GATEWAY, sizeof(GATEWAY));

    loadString(doc["mqtt_server"], MQTT_SERVER, sizeof(MQTT_SERVER));
    loadString(doc["mqtt_user"], MQTT_USER, sizeof(MQTT_USER));
    loadString(doc["mqtt_pass"], MQTT_PASS, sizeof(MQTT_PASS));
    MQTT_PORT = static_cast<uint16_t>(loadInt(doc["mqtt_port"], MQTT_PORT));

    deviceName = DEVICE_NAME;

    writeLog("INFO",
             String("Network config loaded: mode=") +
             (isWifiApMode ? "STA" : "AP") +
             ", ip=" + (isIpConfigStatic ? "STATIC" : "DHCP") +
             ", hostname=" + HOSTNAME);

    return true;
}

IPAddress parseIP(const char* value)
{
    IPAddress address;
    if (value != nullptr && address.fromString(value))
        return address;

    return IPAddress(0, 0, 0, 0);
}

void setupStaticIP()
{
    if (!isIpConfigStatic)
        return;

    const IPAddress localIP = parseIP(IP_ADDR);
    const IPAddress subnet = parseIP(SUBNET_MASK);
    const IPAddress gateway = parseIP(GATEWAY);

    if (localIP == IPAddress(0, 0, 0, 0) ||
        subnet == IPAddress(0, 0, 0, 0) ||
        gateway == IPAddress(0, 0, 0, 0))
    {
        writeLog("WARN", "Static IP config is invalid; using DHCP");
        return;
    }

    if (!WiFi.config(localIP, gateway, subnet))
        writeLog("ERROR", "Failed to apply static IP configuration");
}

void setupAccessPoint()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(DEVICE_NAME, DEVICE_PASS, AP_CHANNEL, false, AP_MAX_CLIENTS);

    writeLog("INFO",
             String("AP started: SSID=") + WiFi.softAPSSID() +
             " IP=" + WiFi.softAPIP().toString());
}

void setupStation()
{
    if (WIFI_SSID[0] == '\0')
    {
        writeLog("WARN", "WiFi SSID is empty; starting AP mode");
        isWifiApMode = false;
        setupAccessPoint();
        return;
    }

    setupStaticIP();

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.setHostname(HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    writeLog("INFO", String("Connecting WiFi: ") + WIFI_SSID);
}

}

void network_setup()
{
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);

    loadNetworkConfig();

    if (isWifiApMode)
        setupStation();
    else
        setupAccessPoint();

    if (MDNS.begin(HOSTNAME))
        writeLog("INFO", String("mDNS started: ") + HOSTNAME + ".local");
    else
        writeLog("WARN", "mDNS start failed");
}

void keepWiFiAlive()
{
    if (!isWifiApMode || WiFi.status() == WL_CONNECTED)
        return;

    const uint32_t now = millis();
    if (now - lastReconnectAttempt < RECONNECT_INTERVAL_MS)
        return;

    lastReconnectAttempt = now;
    WiFi.reconnect();
}

void apModeCheck()
{
    if (digitalRead(AP_PIN) != LOW)
        return;

    if (isWifiApMode)
    {
        WiFi.disconnect(true);
        isWifiApMode = false;
        setupAccessPoint();
        writeLog("INFO", "AP button pressed; switched to AP mode");
    }
}
