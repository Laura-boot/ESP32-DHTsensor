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
#include <LiquidCrystal_I2C.h>
#include "characters.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

// make some custom characters:

void setup() 
{
  lcd.init();
  // Make sure backlight is on       
  lcd.backlight();

  // create a new characters
  lcd.createChar(1, Check);
  lcd.createChar(2, Speaker);
  lcd.createChar(3, Sound);
  lcd.createChar(4, Mute);

  // Clears the LCD screen
  lcd.clear();

  // Print a message to the lcd.
  lcd.print("Custom Character");
}

// Print All the custom characters
void loop() 
{ 
  lcd.setCursor(2, 1);
  lcd.write(1);

  lcd.setCursor(4, 1);
  lcd.write(2);

  lcd.setCursor(5, 1);
  lcd.write(3);

  lcd.setCursor(7, 1);
  lcd.write(2);

  lcd.setCursor(8, 1);
  lcd.write(4);
}