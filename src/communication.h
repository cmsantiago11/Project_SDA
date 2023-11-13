#ifndef COMMUNICATION_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include "esp_system.h"

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

void init_mqtt(const char* servidor){
  client.setServer(servidor,1883);
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

#endif