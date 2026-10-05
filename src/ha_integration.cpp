#include "ha_integration.h"
#include "pzem_monitor.h"
#include "expense_manager.h"

WiFiClient espClient;
PubSubClient client(espClient);

namespace {
const char *device_id = "esp32_powermeter_pzem004";
const char *discovery_prefix = "homeassistant";
const char *availability_topic = "homeassistant/sensor/powermeter_pzem004/availability";
const char *state_topic = "homeassistant/sensor/powermeter_pzem004/state";
unsigned long lastMsg = 0;

String makeObjectId(const char *stateKey) {
    // Home Assistant entity_id: sensor.<DEVICE_NAME>_<state_key>
    // Example: DEVICE_NAME="PowerMeter-PZEM004" -> sensor.powermeter_pzem004_current
    String id = String(DEVICE_NAME) + "_" + stateKey;
    id.toLowerCase();
    for (size_t i = 0; i < id.length(); ++i) {
        char c = id[i];
        if (!isalnum(static_cast<unsigned char>(c)) && c != '_') id.setCharAt(i, '_');
    }
    return id;
}
}

void iotHAsetup() {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setBufferSize(4096);
    client.setKeepAlive(60);
    client.setSocketTimeout(3);
}

void send_sensor_config(const char *state_key, const char *name, const char *unit,
                        const char *device_class, const char *icon, const char *category) {
    String config_topic = String(discovery_prefix) + "/sensor/" + device_id + "/" + state_key + "/config";
    String object_id = makeObjectId(state_key);

    JsonDocument doc;
    doc["name"] = name;
    doc["object_id"] = object_id;
    doc["state_topic"] = state_topic;

    String templateValue = String("{{ value_json.") + state_key + " }}";
    doc["value_template"] = templateValue;
    doc["unique_id"] = object_id;

    if (unit && *unit) doc["unit_of_measurement"] = unit;
    if (device_class && *device_class) doc["device_class"] = device_class;
    if (icon && *icon) doc["icon"] = icon;
    if (category && *category) doc["entity_category"] = category;

    doc["availability_topic"] = availability_topic;
    doc["payload_available"] = "online";
    doc["payload_not_available"] = "offline";

    JsonObject dev = doc["device"].to<JsonObject>();
    JsonArray ids = dev["identifiers"].to<JsonArray>();
    ids.add(device_id);
    dev["name"] = DEVICE_NAME;
    dev["sw_version"] = D_SoftwareVersion;
    dev["manufacturer"] = D_Mfac;
    dev["model"] = D_Model;

    char buffer[1536];
    serializeJson(doc, buffer, sizeof(buffer));
    client.publish(config_topic.c_str(), buffer, true);
}

void send_ha_discovery() {
    send_sensor_config("voltage", "Voltage", "V", "voltage", "mdi:flash", "");
    send_sensor_config("current", "Current", "A", "current", "mdi:current-ac", "");
    send_sensor_config("power", "Power", "W", "power", "mdi:flash", "");
    send_sensor_config("energy", "Energy", "kWh", "energy", "mdi:meter-electric", "");
    send_sensor_config("frequency", "Frequency", "Hz", "frequency", "mdi:sine-wave", "");
    send_sensor_config("pf", "Power Factor", "", "power_factor", "mdi:angle-acute", "");
    send_sensor_config("estimated_bill", "Estimated Bill", "THB", "", "mdi:cash-minus", "");
    send_sensor_config("energy_today", "Energy Today", "kWh", "energy", "mdi:calendar-today", "");
    send_sensor_config("energy_month", "Energy Month", "kWh", "energy", "mdi:calendar-month", "");
    send_sensor_config("ip_address", "IP Address", "", "", "mdi:ip-network", "diagnostic");
    send_sensor_config("mac_address", "MAC Address", "", "", "mdi:lan-connect", "diagnostic");
    send_sensor_config("rssi", "WiFi Signal", "dBm", "signal_strength", "mdi:wifi", "diagnostic");
    send_sensor_config("uptime", "Uptime", "", "timestamp", "mdi:clock", "diagnostic");
}

void publish_all_states() {
    const auto &d = getPzemData();
    String expenseJson;
    buildExpenseJson(expenseJson);
    JsonDocument expense;
    deserializeJson(expense, expenseJson);

    JsonDocument doc;
    doc["voltage"] = d.voltage;
    doc["current"] = d.current;
    doc["power"] = d.power;
    doc["energy"] = d.energy;
    doc["frequency"] = d.frequency;
    doc["pf"] = d.pf;
    doc["energy_today"] = expense["energyToday"] | 0.0f;
    doc["energy_month"] = expense["energyMonth"] | 0.0f;
    doc["estimated_bill"] = expense["estimatedBill"] | 0.0f;
    doc["ip_address"] = WiFi.localIP().toString();
    doc["mac_address"] = WiFi.macAddress();
    doc["rssi"] = WiFi.RSSI();

    time_t nowSec;
    time(&nowSec);
    if (nowSec > 1577836800) {
        time_t bootTime = nowSec - (millis() / 1000);
        struct tm timeinfo;
        gmtime_r(&bootTime, &timeinfo);
        char bootTimeStr[25];
        strftime(bootTimeStr, sizeof(bootTimeStr), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
        doc["uptime"] = bootTimeStr;
    }

    char buffer[2048];
    serializeJson(doc, buffer, sizeof(buffer));
    client.publish(state_topic, buffer, true);
    client.publish(availability_topic, "online", true);
}

void reconnect() {
    static unsigned long lastReconnectAttempt = 0;
    const unsigned long now = millis();
    if (client.connected() || now - lastReconnectAttempt < 5000) return;

    lastReconnectAttempt = now;
    Serial.print("[MQTT] Connecting...");
    if (client.connect(DEVICE_NAME, MQTT_USER, MQTT_PASS, availability_topic, 0, true, "offline")) {
        Serial.println(" connected");
        send_ha_discovery();
        client.publish(availability_topic, "online", true);
        publish_all_states();
    } else {
        Serial.printf(" failed, rc=%d\n", client.state());
    }
}

void iotHAloop() {
    if (WiFi.status() != WL_CONNECTED) return;
    if (!client.connected()) reconnect();
    client.loop();

    const unsigned long now = millis();
    if (client.connected() && now - lastMsg >= 3000) {
        lastMsg = now;
        publish_all_states();
    }
}
