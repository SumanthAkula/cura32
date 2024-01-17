#ifndef CURA32_H
#define CURA32_H

#include "driver/twai.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include <FastLED.h>
#include "FastLED_RGBW.h"

#include "car.h"
#include "cancontroller.h"

#define LED_COUNT 8
#define LED_PIN 6
#define LED_MAX_PROGRESS 2048
#define DEBUG_LED_PIN 48
#define BUTTON_PIN 0

#define DEVICE_NAME "Cura OBD"
#define CAR_SERVICE_UUID "19B10000-E8F2-537E-4F6C-D104768A1214"
#define CAN_FRAME_CHARACTERISTIC_UUID "19B10005-E8F2-537E-4F6C-D104768A1214"
#define DISPLAY_MODE_CHARACTERISTIC_UUID "19B10004-E8F2-537E-4F6C-D104768A1214"

BLEServer *server;
BLEService *car_service;
BLECharacteristic *can_frame_characteristic;     // READ PERMISSION
BLECharacteristic *display_mode_characteristic;  // WRITE PERMISSION
bool ble_connected;		// DEFAULT FALSE
bool prioritizeSpeed;	// DEFAULT FALSE; send only frames with ID 777 if true

class DisplayModeCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    prioritizeSpeed = *((bool *)pCharacteristic->getData());
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    Serial.println("Central connected");
    FastLED.show();
    ble_connected = true;
  }

  void onDisconnect(BLEServer *pServer) override {
    Serial.println("Central disconnected");
    FastLED.show();
    ble_connected = false;

    BLEAdvertising *pAdvertising = server->getAdvertising();
    pAdvertising->addServiceUUID(CAR_SERVICE_UUID);
    pAdvertising->start();
  }
};

CRGBW leds[LED_COUNT];
CRGB *ledsp = (CRGB *)&leds[0];
CRGB debug_led[1];

Car car;

#endif
