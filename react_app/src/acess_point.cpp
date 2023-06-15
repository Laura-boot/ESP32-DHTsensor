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

// Defina as informações do ponto de acesso
const char* ssid = "Lau"; //criou um nome de usuário
const char* pass = "12345678"; //criou uma senha para a rede

// Pino do botão de ligar/desligar
const int pinBotao = 33;

// Variável para controlar o estado do ponto de acesso
bool pontoAcessoAtivo = false;

// procedimento :3
void conect_wifi(bool estadoBotao, const char* ssid, const char* pass){
  // Se o botão estiver pressionado e o ponto de acesso estiver desligado,
  // liga o ponto de acesso. Se o botão estiver pressionado e o ponto de acesso
  // estiver ligado, desliga o ponto de acesso.

  if (estadoBotao == HIGH) {
    if (!pontoAcessoAtivo) {
      WiFi.softAP(ssid, pass);
      IPAddress ip = WiFi.softAPIP();
      pontoAcessoAtivo = true;
      Serial.println("Ponto de acesso WiFi ligado");
      Serial.print("Endereço IP do Ponto de Acesso: ");
      Serial.println(ip); // aparece o ip para conectar no outro esp
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      WiFi.softAPdisconnect();
      pontoAcessoAtivo = false;
      Serial.println("");
      Serial.println("Ponto de acesso WiFi desligado");
      digitalWrite(LED_BUILTIN, LOW);
    }

    // Aguarda um curto período para evitar múltiplas leituras rápidas
    delay(1000);
  }
}

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Inicializa os pinos
  pinMode(pinBotao, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Verifica o estado do botão
  int estadoBotao = digitalRead(pinBotao);
  // Liga ou desliga o Ponto de Acesso a depender do estado do botão
  conect_wifi(estadoBotao, ssid, pass);
}
