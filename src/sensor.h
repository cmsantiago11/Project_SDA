#ifndef SENSOR_H

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

uint8_t getBatteryLevel(BLEClient* client) {
  BLERemoteService* pService = client->getService("0000180F-0000-1000-8000-00805F9B34FB");
  BLERemoteCharacteristic* pCharacteristic = pService->getCharacteristic("00002A19-0000-1000-8000-00805F9B34FB"); //battery level
  std::string raw_value = pCharacteristic->readValue();
  return uint8_t(raw_value[0]);
}

float getTemperature(BLEClient* client) {
  BLERemoteService* pService = client->getService("0000181A-0000-1000-8000-00805F9B34FB");
  BLERemoteCharacteristic* pCharacteristic = pService->getCharacteristic("00002A1F-0000-1000-8000-00805F9B34FB");//Temperature
  std::string temperatura = pCharacteristic->readValue();
  uint16_t raw_value = (temperatura[1] << 8) | (temperatura[0]);
  return float(raw_value) / 10.0;
}

float getHumidity(BLEClient* client) {
  BLERemoteService* pService = client->getService("0000181A-0000-1000-8000-00805F9B34FB");
  BLERemoteCharacteristic* pCharacteristic = pService->getCharacteristic("00002A6F-0000-1000-8000-00805F9B34FB");//Temperature
  std::string humidity = pCharacteristic->readValue();
  uint16_t raw_value = (humidity[1] << 8) | (humidity[0]);
  return float(raw_value) / 100;
}

#endif