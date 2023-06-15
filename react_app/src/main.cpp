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
bool muteAtivo = false;

// DHT config
#define DHTPIN 19   // Define o pino do sensor DHT22
#define DHTTYPE DHT11   // Define o tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

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
const char* ssid = "Lau";
const char* password = "12345678";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

/* -------------------------- procedimentos :3 --------------------------*/
// apaga tudo
void leds_off(){
  digitalWrite(LEDtemp    , LOW);
  digitalWrite(LEDhumi    , LOW);
  digitalWrite(LEDok      , LOW);
  digitalWrite(LEDhigh    , LOW);
}

// Initialize SPIFFS
void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
   Serial.println("SPIFFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');

    lcd.setCursor(1, 0);
    lcd.print("Connecting to ");
    lcd.setCursor(4, 1);
    lcd.print("WiFi");
    lcd.setCursor(9, 1);
    lcd.print(".");
    delay(1000);
    lcd.setCursor(10, 1);
    lcd.print(".");
    delay(1000);
    lcd.setCursor(11, 1);
    lcd.print(".");
    delay(1000);
    lcd.clear();
  }
  Serial.println();
  Serial.println(WiFi.localIP());
}

void on_off_webServer(bool estadoBotao){
  // Se o botão estiver pressionado e o web server estiver desligado,
  // liga o web server. Se o botão estiver pressionado e o web server
  // estiver ligado, desliga o web server.
  /*lcd.init();       
  // create a new characters
  lcd.createChar(1, Check);
  lcd.createChar(2, Speaker);
  lcd.createChar(3, Sound);
  lcd.createChar(4, Mute);
  lcd.clear(); // Clears the LCD screen*/

  if (estadoBotao == HIGH) {
    if (!webServerAtivo) {
      lcd.backlight(); // Make sure backlight is on
      // Print a initial message to the lcd.
      lcd.setCursor(2, 0);
      lcd.print("BEM VINDO(A)");
      lcd.setCursor(7, 1);
      lcd.print(":)");
      delay(3000);
      lcd.clear();
      initFS();
      initWiFi();
      lcd.setCursor(1, 0);
      lcd.print("Connected to ");
      lcd.setCursor(5, 1);
      lcd.print("WiFi");
      lcd.setCursor(10, 1);
      lcd.write(1);
      delay(3000);
      lcd.clear();
      lcd.setCursor(7, 0);
      lcd.print("IP");
      lcd.setCursor(3, 1);
      lcd.print(WiFi.localIP());
      webServerAtivo = true;
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      lcd.clear();
      lcd.noBacklight(); // Make sure backlight is off
      webServerAtivo = false;
      digitalWrite(LED_BUILTIN, LOW);
      leds_off();
    }

    // Aguarda um curto período para evitar múltiplas leituras rápidas
    delay(1000);
  }
}

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

void check_temp_humi(const char* name, float unid, int lim){
  if (isnan(unid)){ // FALHA NA LEITURA DE DADOS
    Serial.println("falha ao ler dados do sensor DHT11");
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

void getTempPage(AsyncWebServerRequest *request) {
  float temp = dht.readTemperature();
  digitalWrite(LEDtemp, HIGH);
  digitalWrite(LEDhumi, LOW);
  Serial.print("temp: ");
  Serial.print(temp);
  Serial.println("°C ");
  request->send(SPIFFS, "/temp.html", "text/html");
}

void getHumiPage(AsyncWebServerRequest *request) {
  float humi = dht.readHumidity();
  digitalWrite(LEDtemp, LOW);
  digitalWrite(LEDhumi, HIGH);
  Serial.print("humi: ");
  Serial.print(humi);
  Serial.println("% ");
  request->send(SPIFFS, "/humi.html", "text/html");
}

void handleTemp(AsyncWebServerRequest *request) {
  float temp = dht.readTemperature();   // Lê a temperatura do sensor
  check_temp_humi("TEMP",temp, lim_temp);
  String TempValue = String(temp);
  request->send(200, "text/plane", TempValue); //Send temp value only to client ajax request
}

void handleHumi(AsyncWebServerRequest *request) {
  float humi = dht.readHumidity();   // Lê a temperatura do sensor
  check_temp_humi("HUMI",humi, lim_humi);
  String HumiValue = String(humi);
  request->send(200, "text/plane", HumiValue); //Send temp value only to client ajax request
}


void setup(){
  Serial.begin(115200);

  pinMode(LEDtemp    , OUTPUT);
  pinMode(LEDhumi    , OUTPUT);
  pinMode(LEDok      , OUTPUT);
  pinMode(LEDhigh    , OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  leds_off();

  pinMode(buttonWeb, INPUT);
  pinMode(buttonAlarm, INPUT);

  dht.begin();

  lcd.init();       
  // create a new characters
  lcd.createChar(1, Check);
  lcd.createChar(2, Speaker);
  lcd.createChar(3, Sound);
  lcd.createChar(4, Mute);
  lcd.clear(); // Clears the LCD screen

  //initFS();
  on_off_webServer(digitalRead(buttonWeb));
  //initWiFi();
  // digitalWrite(LED_BUILTIN, HIGH);

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("HOME");
    lcd.setCursor(0, 1);
    lcd.write(2);
    on_off_mute();       // sound state function
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("HOME");
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.setCursor(1, 1);
    on_off_mute();       // sound state function
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/help.html", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("HELP");
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.setCursor(1, 1);
    on_off_mute();       // sound state function
    request->send(SPIFFS, "/help.html", "text/html");
  });

  server.on("/test.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/test.html", "text/html");
  });
  
  server.on("/temp.html", HTTP_GET, getTempPage); //To get update of temp value only
  server.on("/temp/getTemp", HTTP_GET, handleTemp);

  server.on("/humi.html", HTTP_GET, getHumiPage); //To get update of humi value only
  server.on("/humi/getHumi", HTTP_GET, handleHumi);

  server.serveStatic("/", SPIFFS, "/");
  
  server.begin(); //Start server
}

void loop() {
  // Verifica o estado do botão
  int estadoButtonWeb = digitalRead(buttonWeb);

  // Liga ou desliga o Ponto de Acesso a depender do estado do botão
  on_off_webServer(estadoButtonWeb);
}