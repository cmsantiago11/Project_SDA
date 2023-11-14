#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include "esp_system.h"

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#include "sensor.h"
#include "communication.h"
#include "scan.h"

const char* ssid = "xxxxxxx";
const char* password = "xxxxxx";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;


BLEClient*  pClient;

void setup() {
  Serial.begin(115200);
  init_wifi(ssid, password);
  init_mqtt(mqttServer);
  pClient = scan_Devices(pClient);
}

void loop() {
  float temp = getTemperature(pClient);
  float hum = getHumidity(pClient);
  uint8_t bat = getBatteryLevel(pClient);
  send_Json(temp, hum, bat);
  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("Hum: "); Serial.println(hum);
  Serial.print("Bat: "); Serial.println(bat);
  delay(20000);
}