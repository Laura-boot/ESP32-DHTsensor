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

// Pino do botão de ligar/desligar web server
int buttonWeb = 35;

// Pino do botão de ligar/desligar alarm 
int buttonAlarm = 34;

// Variável para controlar o estado do web server
bool webServerAtivo = false;

// Variável para controlar o estado do alarm
bool alarmAtivo = false;

// procedimento :3
void on_off_webServer(bool estadoBotao){
  // Se o botão estiver pressionado e o web server estiver desligado,
  // liga o web server. Se o botão estiver pressionado e o web server
  // estiver ligado, desliga o web server.

  if (estadoBotao == HIGH) {
    if (!webServerAtivo) {
      webServerAtivo = true;
      Serial.println("Web server ligado");
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      webServerAtivo = false;
      Serial.println("");
      Serial.println("Web server desligado");
      digitalWrite(LED_BUILTIN, LOW);
    }

    // Aguarda um curto período para evitar múltiplas leituras rápidas
    delay(1000);
  }
}

void on_off_alarm(bool estadoBotao){
  // Se o botão estiver pressionado e o alarm estiver desligado,
  // liga o alarm. Se o botão estiver pressionado e o alarm
  // estiver ligado, desliga o alarm.

  if (estadoBotao == HIGH) {
    if (!alarmAtivo) {
      alarmAtivo = true;
      Serial.println("alarm ligado");
    } else {
      alarmAtivo = false;
      Serial.println("");
      Serial.println("alarm desligado");
    }

    // Aguarda um curto período para evitar múltiplas leituras rápidas
    delay(1000);
  }
}

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Inicializa os pinos
  pinMode(buttonWeb, INPUT);
  pinMode(buttonAlarm, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Verifica os estados dos botões
  int estadoButtonWeb = digitalRead(buttonWeb);
  int estadoButtonAlarm = digitalRead(buttonAlarm);
  // Liga ou desliga o Ponto de Acesso a depender do estado do botão
  on_off_webServer(estadoButtonWeb);
  on_off_alarm(estadoButtonAlarm);
}
