#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_system.h"
#include <sstream>

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
//const char* mqttUser = "SDA_ESP32";
//const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

void init_wifi(const char* nombre_red,const char* contrasena){
  WiFi.begin(nombre_red, contrasena);
  //Serial.println("...................................");

  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED){  
    delay(500);
    //Serial.print(".") ;
  }
  Serial.print("Connected to the WiFi network: ");
  Serial.println(ssid);
}

void init_mqtt(const char* servidor,const int port){
  client.setServer(servidor,port);
  while (!client.connected()){      
    Serial.print("Connecting to MQTT...");
  }
  Serial.println();
}

void send_Json(float temp, float hum, float bat){
  StaticJsonDocument<100> jsonDoc;
  jsonDoc["t"] = temp;
  jsonDoc["h"] = hum;
  jsonDoc["b"] = bat;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  client.publish("Alic/MQTT", jsonString.c_str());

  Serial.println(jsonString);
  delay(800);
}


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    if (advertisedDevice.haveName() && advertisedDevice.haveServiceData() && !advertisedDevice.getName().compare("ATC_6B3C77")) {

      int serviceDataCount = advertisedDevice.getServiceDataCount();
      std::string strServiceData = advertisedDevice.getServiceData(0);

      uint8_t* 	Payload=advertisedDevice.getPayload();
      size_t		PayloadLenght= advertisedDevice.getPayloadLength();

      Serial.println(PayloadLenght);

        if (Payload != nullptr) {
          // Recorre los elementos del array y envíalos al puerto serie
          for (int i = 0; i < PayloadLenght; ++i) {
              Serial.print(Payload[i], HEX); // Imprime el valor en formato hexadecimal
              Serial.print(" "); // Agrega un espacio para separar los valores
          }
          Serial.println(); // Agrega una nueva línea al final para una mejor legibilidad
        } else {
          Serial.println("El puntero es NULL"); // Maneja el caso si el puntero es NULL
        }

      Serial.printf("\n\nAdvertised Device: %s\n", advertisedDevice.toString().c_str());
      Serial.println();
      
      char cTemp1[3];
      sprintf(cTemp1, "%02X", Payload[10]);
      char cTemp2[3];
      sprintf(cTemp2, "%02X", Payload[11]);

      //Serial.print("Temp en HEX es: ");
      //Serial.print(cTemp1);
      //Serial.print("  ");
      //Serial.println(cTemp2);
//
      char cHum[3];
      //sprintf(cHum, "%02X", Payload[12]);
      //Serial.print("Hum en HEX es: ");
      //Serial.println(cHum);
//
      char cBat[3];
      //sprintf(cBat, "%02X", Payload[13]);
      //Serial.print("Batt en HEX es: ");
      //Serial.println(cBat);
      //Serial.println();

      long Temperature = 255*strtol(cTemp1, NULL, 16)+strtol(cTemp2, NULL, 16);
      float Temp = Temperature/10.0;
      Serial.print("Temperature: ");
      Serial.println(Temp);

      long Humidity = strtol(cHum, NULL, 16);
      Serial.print("Humidity: ");
      Serial.println(Humidity);

      long Battery = strtol(cBat, NULL, 16);
      Serial.print("Battery level: ");
      Serial.println(Battery);
      Serial.println();

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
  Serial.println("ESP32 XIAOMI DISPLAY");
  init_wifi(ssid,password);
  init_mqtt(mqttServer,mqttPort);
  initBluetooth();
}



void loop() {
  
  char printLog[256];
  Serial.printf("Start BLE scan for %d seconds...\n", SCAN_TIME);
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME);
  int count = foundDevices.getCount();
  printf("Found device count : %d\n", count);

  delay(100);
}