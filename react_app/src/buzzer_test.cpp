/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-piezo-buzzer
 */
#include <Wire.h>
#include <Arduino.h>
#include "FS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <painlessMesh.h>
#include <Arduino_JSON.h>
//#include <LiquidCrystal_I2C.h>
//#include "characters.h"

#include "pitches.h"
#include "Arduino.h"
#define BUZZER_PIN 33  // ESP32 pin GIOP18 connected to piezo buzzer

int melody[] = {
  NOTE_E6, NOTE_C6
};

int noteDurations[] = {
  150, 150
};

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  tone(BUZZER_PIN, melody[0]);
  delay(200);
  tone(BUZZER_PIN, melody[1]);
  delay(200);
}
