#ifndef SCAN_H

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

BLEClient* scan_Devices(BLEClient*  pClient){
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
      pBLEScan->clearResults();
      return pClient;
    } 
  }
}













#endif