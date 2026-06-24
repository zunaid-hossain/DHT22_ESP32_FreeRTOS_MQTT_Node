# DHT22_ESP32_FreeRTOS_MQTT_Node

## Phase 1 – Task A: Resilient ESP32 Environmental Node

### Overview

This project implements a resilient environmental monitoring node using an ESP32, FreeRTOS, MQTT, and a DHT22 sensor.

The ESP32 periodically reads temperature and humidity data every 5 seconds, adds small fluctuations to simulate real-world sensor variations, serializes the data into JSON format, and publishes it to a public MQTT broker.

The firmware is designed around a non-blocking architecture that automatically recovers from WiFi and MQTT failures without using `delay()`, ensuring continuous system responsiveness.

---

## Project Objectives

* Read environmental data from a DHT22 sensor
* Publish telemetry data every 5 seconds
* Serialize data using JSON
* Connect to a public MQTT broker
* Implement non-blocking WiFi recovery
* Implement non-blocking MQTT recovery
* Demonstrate resilience against network failures
* Support testing using MQTTX and Wokwi

---

## Features

### Core Requirements

* ESP32 FreeRTOS Dual-Core Architecture
* DHT22 Temperature & Humidity Monitoring
* Simulated Sensor Fluctuation
* JSON Serialization using ArduinoJson
* MQTT Communication using PubSubClient
* Automatic WiFi Reconnection
* Automatic MQTT Reconnection
* MQTT Last Will and Testament (LWT)
* Non-Blocking Firmware Design
* Wokwi Simulation Support

### Additional Features

* Remote LED Control via MQTT Commands
* Device Status Reporting (Online/Offline)
* WiFi RSSI Monitoring
* Uptime Tracking
* State Machine Monitoring

---

## System Architecture

### Core 0 – WiFi Management Task

Responsibilities:

* WiFi connection monitoring
* WiFi reconnection
* Network status updates

### Core 1 – MQTT & Sensor Task

Responsibilities:

* MQTT connection management
* MQTT message processing
* Sensor acquisition
* JSON serialization
* Telemetry publishing

---

## FreeRTOS Task Structure

### Task 1: WiFi_Reconnect_Task

**Core:** 0

**Purpose:**

* Monitor WiFi connection status
* Automatically reconnect if disconnected
* Update system state

### Task 2: MQTT_Sensor_Task

**Core:** 1

**Purpose:**

* Maintain MQTT connection
* Publish telemetry data
* Process incoming MQTT messages
* Handle sensor readings

---

## State Machine

The firmware uses a simple state machine for connection management.

```text
STATE_WIFI_CONNECTING
        ↓
STATE_WIFI_CONNECTED
        ↓
STATE_MQTT_CONNECTING
        ↓
STATE_MQTT_CONNECTED
```

### State Descriptions

| State           | Description                 |
| --------------- | --------------------------- |
| WIFI_CONNECTING | Attempting WiFi connection  |
| WIFI_CONNECTED  | WiFi connected successfully |
| MQTT_CONNECTING | Attempting MQTT connection  |
| MQTT_CONNECTED  | WiFi and MQTT operational   |

---

## MQTT Topics

### Telemetry Topic

```text
esp32/wokwi/freertos/data
```

Publishes sensor data every 5 seconds.

### Command Topic

```text
esp32/wokwi/freertos/cmd
```

Accepted commands:

```text
ON
OFF
```

### Status Topic

```text
esp32/wokwi/freertos/status
```

Published values:

```text
online
offline
```

---

## Example JSON Payload

```json
{
  "device_id": "ESP32_Wokwi_FreeRTOS_Node_01",
  "temperature": 26.7,
  "humidity": 63.2,
  "wifi_rssi": -58,
  "uptime_sec": 125,
  "state": "MQTT_CONNECTED",
  "task_core": 1
}
```

---

## Non-Blocking Reconnection Strategy

### WiFi Recovery

The WiFi task continuously monitors:

```cpp
WiFi.status()
```

If WiFi disconnects:

```cpp
WiFi.disconnect(false);
WiFi.begin(ssid, password);
```

No blocking delays are used.

Task scheduling is handled through:

```cpp
vTaskDelay()
```

which allows other FreeRTOS tasks to continue executing.

---

### MQTT Recovery

MQTT reconnection attempts occur every:

```text
5000 ms
```

using:

```cpp
millis()
```

instead of blocking delays.

Pseudo workflow:

```cpp
if (!client.connected())
{
    if (millis() - lastReconnect >= 5000)
    {
        connectMQTT();
    }
}
```

This ensures the device remains responsive during broker outages.

---

## Synchronization

A FreeRTOS mutex is used to protect MQTT operations:

```cpp
SemaphoreHandle_t mqttMutex;
```

This prevents concurrent access to the MQTT client and improves reliability.

---

## Hardware Connections

| Component  | ESP32 Pin |
| ---------- | --------- |
| DHT22 Data | GPIO 15   |
| LED        | GPIO 2    |
| VCC        | 3.3V      |
| GND        | GND       |

---

## Libraries Used

```text
WiFi.h
PubSubClient.h
ArduinoJson.h
DHTesp.h
```

Install all libraries using the Arduino Library Manager before compiling.

---

## Development Environment

### Board

ESP32 Dev Module

### Framework

Arduino Framework

### Language

C++

### IDE

* Arduino IDE 2.x
* PlatformIO

---

## Compilation Instructions

### Arduino IDE

1. Install ESP32 Board Package.
2. Install required libraries:

   * PubSubClient
   * ArduinoJson
   * DHTesp
3. Select:

   * Board: ESP32 Dev Module
4. Open `ESP32_wokwi.ino`
5. Compile and Upload

### PlatformIO

1. Clone the repository:

```bash
git clone https://github.com/zunaid-hossain/DHT22_ESP32_FreeRTOS_MQTT_Node.git
```

2. Open the project in PlatformIO.
3. Build and Upload.

---

## Testing with MQTTX

### Subscribe To

```text
esp32/wokwi/freertos/data
```

```text
esp32/wokwi/freertos/status
```

### Publish Commands To

```text
esp32/wokwi/freertos/cmd
```

Example:

```text
ON
```

or

```text
OFF
```

---

## Wokwi Simulation

### Project Link

https://wokwi.com/projects/467724628107981825

---

## Demonstration Checklist

### Sensor Publishing Test

* ESP32 connects to WiFi
* ESP32 connects to MQTT Broker
* JSON payload appears in MQTTX every 5 seconds

### MQTT Command Test

* Publish `ON`
* LED turns ON
* Publish `OFF`
* LED turns OFF

### WiFi Recovery Test

* Disconnect WiFi
* Observe automatic reconnection attempts
* Restore WiFi
* Telemetry resumes automatically

### MQTT Recovery Test

* Disconnect MQTT Broker
* Observe automatic reconnect attempts
* Restore Broker Connection
* MQTT reconnects automatically

---

## Repository Structure

```text
DHT22_ESP32_FreeRTOS_MQTT_Node/
│
├── ESP32_wokwi.ino
├── README.md
├── LICENSE
└── Demo_Video.mp4
```

---

## Deliverables

### Included

* ESP32 Firmware Source Code
* README Documentation
* Wokwi Simulation Project
* MQTTX Demonstration
* FreeRTOS Architecture
* Non-Blocking Reconnection State Machine

---

## Author

**Md. Zunaid Hossain**
Internet of Things and Robotics Engineering
University of Frontier Technology Bangladesh (UFTB)

### Phase 1 Practical Take-Home Task

**Task A: Resilient ESP32 Environmental Node**
