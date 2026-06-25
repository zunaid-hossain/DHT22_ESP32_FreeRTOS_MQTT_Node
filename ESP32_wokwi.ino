#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHTesp.h"

#define LED_PIN 2
#define DHT_PIN 15

#define LED_ON  HIGH
#define LED_OFF LOW

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

const char* device_id = "ESP32_Wokwi_FreeRTOS_Node_01";

const char* topic_telemetry = "esp32/wokwi/freertos/data";
const char* topic_command   = "esp32/wokwi/freertos/cmd";
const char* topic_status    = "esp32/wokwi/freertos/status";

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dhtSensor;

SemaphoreHandle_t mqttMutex;

unsigned long lastSensorPublish = 0;
unsigned long lastMqttReconnect = 0;

const unsigned long publishInterval = 5000;
const unsigned long mqttReconnectInterval = 5000;

enum SystemState {
  STATE_WIFI_CONNECTING,
  STATE_WIFI_CONNECTED,
  STATE_MQTT_CONNECTING,
  STATE_MQTT_CONNECTED
};

volatile SystemState currentState = STATE_WIFI_CONNECTING;

String getStateName() {
  switch (currentState) {
    case STATE_WIFI_CONNECTING: return "WIFI_CONNECTING";
    case STATE_WIFI_CONNECTED: return "WIFI_CONNECTED";
    case STATE_MQTT_CONNECTING: return "MQTT_CONNECTING";
    case STATE_MQTT_CONNECTED: return "MQTT_CONNECTED";
    default: return "UNKNOWN";
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.trim();

  Serial.print("[MQTT] Topic: ");
  Serial.println(topic);
  Serial.print("[MQTT] Message: ");
  Serial.println(message);

  if (String(topic) == topic_command) {
    if (message == "ON") {
      digitalWrite(LED_PIN, LED_ON);
      Serial.println("[LED] ON");
      client.publish(topic_telemetry, "{\"event\":\"LED_ON\"}");
    } 
    else if (message == "OFF") {
      digitalWrite(LED_PIN, LED_OFF);
      Serial.println("[LED] OFF");
      client.publish(topic_telemetry, "{\"event\":\"LED_OFF\"}");
    }
  }
}

bool connectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  Serial.println("[MQTT] Attempting connection...");

  bool connected = client.connect(
    device_id,
    topic_status,
    1,
    true,
    "offline"
  );

  if (connected) {
    Serial.println("[MQTT] Connected");

    client.publish(topic_status, "online", true);
    client.subscribe(topic_command);

    currentState = STATE_MQTT_CONNECTED;
  } else {
    Serial.print("[MQTT] Failed, rc=");
    Serial.println(client.state());

    currentState = STATE_MQTT_CONNECTING;
  }

  return connected;
}

void wifiTask(void* parameter) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (true) {
    wl_status_t wifiStatus = WiFi.status();

    if (wifiStatus == WL_CONNECTED) {
      if (currentState == STATE_WIFI_CONNECTING) {
        Serial.println("[Core 0][WiFi] Connected");
        Serial.print("[Core 0][WiFi] IP: ");
        Serial.println(WiFi.localIP());
      }

      if (!client.connected()) {
        currentState = STATE_WIFI_CONNECTED;
      }
    } 
    else {
      currentState = STATE_WIFI_CONNECTING;

      Serial.print("[Core 0][WiFi] Not connected. Status code: ");
      Serial.println(wifiStatus);

      WiFi.disconnect(false);
      WiFi.begin(ssid, password);
    }

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void mqttSensorTask(void* parameter) {
  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {

        if (!client.connected()) {
          unsigned long now = millis();

          if (now - lastMqttReconnect >= mqttReconnectInterval) {
            lastMqttReconnect = now;
            currentState = STATE_MQTT_CONNECTING;
            connectMQTT();
          }
        }

        client.loop();

        unsigned long now = millis();

        if (client.connected() && now - lastSensorPublish >= publishInterval) {
          lastSensorPublish = now;

          TempAndHumidity data = dhtSensor.getTempAndHumidity();

          float simulatedTemp = data.temperature + random(-20, 21) * 0.1;
          float simulatedHum  = data.humidity + random(-30, 31) * 0.1;

          JsonDocument doc;

          doc["device_id"] = device_id;
          doc["temperature"] = simulatedTemp;
          doc["humidity"] = simulatedHum;
          doc["wifi_rssi"] = WiFi.RSSI();
          doc["uptime_sec"] = millis() / 1000;
          doc["state"] = getStateName();
          doc["task_core"] = xPortGetCoreID();

          char payload[256];
          serializeJson(doc, payload);

          Serial.print("[Core 1][MQTT] Publishing: ");
          Serial.println(payload);

          client.publish(topic_telemetry, payload);
        }

        xSemaphoreGive(mqttMutex);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting ESP32 Wokwi FreeRTOS MQTT Node...");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  randomSeed(esp_random());

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  client.setKeepAlive(30);

  mqttMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
    wifiTask,
    "WiFi_Reconnect_Task",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    mqttSensorTask,
    "MQTT_Sensor_Task",
    8192,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
