# DHT22_ESP32_FreeRTOS_MQTT_Node


## Overview

This project implements a resilient ESP32 environmental monitoring node using FreeRTOS. The device periodically reads temperature and humidity data from a DHT22 sensor, serializes the data into JSON format, and publishes it to a public MQTT broker.

The firmware is designed to remain responsive during WiFi and MQTT failures through a non-blocking reconnection strategy.

## Features

* ESP32 Dual-Core FreeRTOS Architecture
* DHT22 Temperature and Humidity Monitoring
* JSON Data Serialization using ArduinoJson
* MQTT Communication using PubSubClient
* Last Will and Testament (LWT)
* Automatic WiFi Reconnection
* Automatic MQTT Reconnection
* Remote Command Handling (LED ON/OFF)
* Wokwi Simulation Support

## System Architecture

Core 0:

* WiFi Monitoring
* WiFi Reconnection

Core 1:

* MQTT Management
* Sensor Acquisition
* JSON Serialization
* Telemetry Publishing

## MQTT Topics

Telemetry:
esp32/wokwi/freertos/data

Commands:
esp32/wokwi/freertos/cmd

Status:
esp32/wokwi/freertos/status

## Example Payload

{
"device_id":"ESP32_Wokwi_FreeRTOS_Node_01",
"temperature":24.5,
"humidity":60.1,
"wifi_rssi":-72,
"uptime_sec":120,
"state":"MQTT_CONNECTED"
}

## State Machine

WIFI_CONNECTING
↓
WIFI_CONNECTED
↓
MQTT_CONNECTING
↓
MQTT_CONNECTED

## Libraries Used

* WiFi.h
* PubSubClient
* ArduinoJson
* DHTesp

## Simulation

Wokwi Project:
[Paste Wokwi Share Link Here]

## Demonstration Video

[Paste Video Link Here]

## Author

Md. Zunaid Hossain
Internet of Things and Robotics Engineering
Bangabandhu Sheikh Mujibur Rahman Digital University, Bangladesh
