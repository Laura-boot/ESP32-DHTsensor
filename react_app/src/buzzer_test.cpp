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
#include <LiquidCrystal_I2C.h>
//#include "characters.h"

#include "pitches.h"
#include "Arduino.h"
#define BUZZER_PIN 33  // ESP32 pin GIOP18 connected to piezo buzzer

// Pino do botão de ligar/desligar alarm 
int buttonAlarm = 34;

// Variável para controlar o estado do alarm
bool muteAtivo = false;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// LEDs config
#define LEDtemp 15
#define LEDhumi 18
#define LEDok   23
#define LEDhigh 4 //porta 4 ou 2

// lim config (by slider control)
#define lim_temp 28
#define lim_humi 60

int melody[] = {
  NOTE_E6, NOTE_C6
};

int noteDurations[] = {
  200, 200
};

void on_off_mute(){
  // Se o botão estiver pressionado e o 'mute' estiver desligado,
  // liga o 'mute'. Se o botão estiver pressionado e o 'mute'
  // estiver ligado, desliga o 'mute'. E atualiza o LDC.

  lcd.backlight(); // Make sure backlight is on

  int estadoBotao = digitalRead(buttonAlarm);
  

  if (estadoBotao == HIGH) {
    if (!muteAtivo) {
      muteAtivo = true;
      //add função que silencia o alarme
      //atualiza o icone do lcd
      lcd.setCursor(1, 1);
      lcd.write(4);
      Serial.println("mute ligado");
    } else {
      muteAtivo = false;
      //add função que desilencia o alarme
      //atualiza o icone do lcd
      lcd.setCursor(1, 1);
      lcd.write(3);
      Serial.println("mute desligado");
    }

    // Aguarda um curto período para evitar múltiplas leituras rápidas
    delay(1000);
  }
}

void alarm(const char* state){
  if (!muteAtivo & state=="high"){
    tone(BUZZER_PIN,melody[0]);
    delay(noteDurations[0]);
    tone(BUZZER_PIN,melody[1]);
    delay(noteDurations[1]);
  }
}

void check_temp_humi(const char* name, float unid, int lim){
  if (isnan(unid)){ // FALHA NA LEITURA DE DADOS
    Serial.println("falha ao ler dados do sensor DHT11");
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(name);
    lcd.setCursor(7, 1);
    lcd.print("erro");
    return;
  }
  else if (unid <= lim){ // TEMP ou HUMI OK
    //Serial.println("testei a temperatura ou a humidade e deu BOM");
    digitalWrite(LEDok, HIGH);
    digitalWrite(LEDhigh, LOW);
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(name);
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.setCursor(1, 1);
    on_off_mute();       // sound state function
    lcd.setCursor(7, 1);
    lcd.print("ok");
  }
  else{
    //Serial.println("testei a temperatura ou a humidade e deu RUIM");
    digitalWrite(LEDok, LOW);
    digitalWrite(LEDhigh, HIGH);
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(name);
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.setCursor(1, 1);
    on_off_mute();       // sound state function
    lcd.setCursor(6, 1);
    lcd.print("high");
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  
}
