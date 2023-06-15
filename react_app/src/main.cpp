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

void check_temp_humi(const char* name, float unid, int lim){
  if (isnan(unid)){ // FALHA NA LEITURA DE DADOS
    Serial.print("falha ao ler dados do sensor DHT11");
    return;
  }
  else if (unid <= lim){ // TEMP ou HUMI OK
    Serial.println("testei a temperatura ou a humidade e deu BOM");
    digitalWrite(LEDok, HIGH);
    digitalWrite(LEDhigh, LOW);
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(name);
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.setCursor(1, 1);
    lcd.write(3);
    lcd.setCursor(7, 1);
    lcd.print("ok");
  }
  else{
    Serial.println("testei a temperatura ou a humidade e deu RUIM");
    digitalWrite(LEDok, LOW);
    digitalWrite(LEDhigh, HIGH);
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(name);
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.setCursor(1, 1);
    lcd.write(4);
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

void leds_off(){
  digitalWrite(LEDtemp    , LOW);
  digitalWrite(LEDhumi    , LOW);
  digitalWrite(LEDok      , LOW);
  digitalWrite(LEDhigh    , LOW);
}


void setup(){
  Serial.begin(115200);

  pinMode(LEDtemp    , OUTPUT);
  pinMode(LEDhumi    , OUTPUT);
  pinMode(LEDok      , OUTPUT);
  pinMode(LEDhigh    , OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  leds_off();

  dht.begin();

  lcd.init();       
  lcd.backlight(); // Make sure backlight is on
  // create a new characters
  lcd.createChar(1, Check);
  lcd.createChar(2, Speaker);
  lcd.createChar(3, Sound);
  lcd.createChar(4, Mute);
  lcd.clear(); // Clears the LCD screen
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

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("HOME");
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("HOME");
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/help.html", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("HELP");
    request->send(SPIFFS, "/help.html", "text/html");
  });
  
  server.on("/temp.html", HTTP_GET, getTempPage); //To get update of temp value only
  server.on("/temp/getTemp", HTTP_GET, handleTemp);

  server.on("/humi.html", HTTP_GET, getHumiPage); //To get update of humi value only
  server.on("/humi/getHumi", HTTP_GET, handleHumi);

  server.serveStatic("/", SPIFFS, "/");
  
  server.begin(); //Start server
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}