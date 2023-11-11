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

const char* ssid = "Azrrael";
const char* password = "3006750425ABC";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;


WiFiClient espClient;
PubSubClient client(espClient);

void init_wifi(const char* nombre_red,const char* contrasena){
  WiFi.begin(nombre_red, contrasena);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");  
    delay(500);
  }
  Serial.println("Conectado a la red WiFi");
}

void init_mqtt(const char* servidor,const int port){
  client.setServer(servidor,port);
  while (!client.connected()){      
    client.connect("ESP32Client_0000w3ds");
    delay(500);
  }
  Serial.println("Conectado al servidor MQTT");
}

void send_Json(float temp, float hum, float bat){
  StaticJsonDocument<100> jsonDoc;
  jsonDoc["t"] = temp;
  jsonDoc["h"] = hum;
  jsonDoc["b"] = bat;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  client.publish("Alic/MQTT", jsonString.c_str());
  delay(800);
}


BLEClient*  pClient;

void setup() {
  Serial.begin(115200);
  init_wifi(ssid, password);
  init_mqtt(mqttServer, mqttPort);


  BLEDevice::init("");
  BLEScan *pBLEScan;
  BLEAdvertisedDevice sensor;

  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
  BLEScanResults foundDevices = pBLEScan->start(5, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  for (int i=0; i<foundDevices.getCount(); i++) {
    if (foundDevices.getDevice(i).getName() == "ATC_6B3C77") {
      pBLEScan->stop();
      Serial.println("Found ATC_6B3C77.. waiting for info");
      BLEAdvertisedDevice sensor = foundDevices.getDevice(i);
      delay(10000);
      pClient  = BLEDevice::createClient();
      pClient->connect(&sensor);
      Serial.println(" - Connected to server");
      break;
    } 
  }
  pBLEScan->clearResults();
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