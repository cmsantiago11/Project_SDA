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



#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

int scanTime = 5; //In seconds
BLEScan *pBLEScan;

BLEClient*  pClient;
BLERemoteCharacteristic* pCharacteristic;

BLEAdvertisedDevice sensor;



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
    client.connect("ESP32Client");
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



void setup() {
  Serial.begin(115200);
  init_wifi(ssid,password);
  init_mqtt(mqttServer,mqttPort);


  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  for (int i=0; i<foundDevices.getCount(); i++) {
    //Serial.println(foundDevices.getDevice(i).getAddress().toString().c_str());
    if (foundDevices.getDevice(i).getName() == "ATC_6B3C77") {
      pBLEScan->stop();
      sensor = foundDevices.getDevice(i);
      Serial.println("Found ATC_6B3C77");
      pClient  = BLEDevice::createClient();
      pClient->connect(&sensor);
      Serial.println(" - Connected to server");
      //pClient->discoverAttributes();
      Serial.println(" - Discovered attributes");
      break;
    } 
  }
  pBLEScan->clearResults(); 
}



void loop() {
  
  BLERemoteService* pService = pClient->getService("0000180F-0000-1000-8000-00805F9B34FB"); 
  pCharacteristic = pService->getCharacteristic("00002A19-0000-1000-8000-00805F9B34FB"); //battery level
  std::string bateria = pCharacteristic->readValue(); 
  BLERemoteService* pService2 = pClient->getService("0000181A-0000-1000-8000-00805F9B34FB");
  pCharacteristic = pService->getCharacteristic("00002A1F-0000-1000-8000-00805F9B34FB");//Temperature
  std::string temperatura = pCharacteristic->readValue(); //temperature=(hex2*255+hex1)/100
  //pCharacteristic = pService2->getCharacteristic("00002A6F-0000-1000-8000-00805F9B34FB");//Humidity
  //std::string humedad = pCharacteristic->readValue(); //humidity=(hex2*2.55)+(hex1/100)
  Serial.println("The battery value was: ");
  Serial.println(bateria.c_str());
  for (size_t i = 0; i < bateria.size(); ++i) {
      Serial.print("0x");
      if (bateria[i] < 0x10) {
        Serial.print("0");  // Agregar un cero inicial para valores menores a 0x10
      }
      Serial.print(bateria[i], HEX);
      Serial.print(" ");
    }
  Serial.println();
  Serial.println("The temperature value was: ");
  Serial.println(temperatura.c_str());
  for (size_t i = 0; i < temperatura.size(); ++i) {
      Serial.print("0x");
      if (temperatura[i] < 0x10) {
        Serial.print("0");  // Agregar un cero inicial para valores menores a 0x10
      }
      Serial.print(temperatura[i], HEX);
      Serial.print(" ");
    }
  Serial.println();
  send_Json(random(100)/10,random(100)/10,random(100)/10);

  delay(3000);
}