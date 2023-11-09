#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include "esp_system.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SCAN_TIME  10 // seconds

boolean METRIC = true; //Set true for metric system; false for imperial
BLEScan *pBLEScan;



const char* ssid = "Calle_Mendez";
const char* password = "Frida1628";
const char* mqttServer = "broker.mqtt.cool";
const int mqttPort = 1883;


WiFiClient espClient;
PubSubClient client(espClient);

void init_wifi(const char* nombre_red,const char* contrasena){
  WiFi.begin(nombre_red, contrasena);

  while (WiFi.status() != WL_CONNECTED){  
    delay(500);
  }
}

void init_mqtt(const char* servidor,const int port){
  client.setServer(servidor,port);
  while (!client.connected()){      

  }
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


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    if (advertisedDevice.haveName() && advertisedDevice.haveServiceData() && !advertisedDevice.getName().compare("ATC_6B3C77")) {

      int serviceDataCount = advertisedDevice.getServiceDataCount();
      std::string strServiceData = advertisedDevice.getServiceData(0);

      uint8_t* 	Payload=advertisedDevice.getPayload();

      //Serial.printf("\n\nAdvertised Device: %s\n", advertisedDevice.toString().c_str());

      
      char cTemp1[3];
      sprintf(cTemp1, "%02X", Payload[10]);
      char cTemp2[3];
      sprintf(cTemp2, "%02X", Payload[11]);

      char cHum[3];
      sprintf(cHum, "%02X", Payload[12]);

      char cBat[3];
      sprintf(cBat, "%02X", Payload[13]);

      float Temperature = 255*strtol(cTemp1, NULL, 16)+strtol(cTemp2, NULL, 16);
      float Temp = Temperature/10.0;

      long Humidity = strtol(cHum, NULL, 16);

      long Battery = strtol(cBat, NULL, 16);

      send_Json(Temp,Humidity,Battery);
    }
  }
};

void initBluetooth()
{
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(0x50);
  pBLEScan->setWindow(0x30);
}



void setup() {
  Serial.begin(115200);
  init_wifi(ssid,password);
  init_mqtt(mqttServer,mqttPort);
  initBluetooth();
}



void loop() {
  
  char printLog[256];
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME);
  int count = foundDevices.getCount();

  delay(100);
}