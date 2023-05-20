#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <painlessMesh.h>
#include <Arduino_JSON.h>

// DHT config
#define DHTPIN 19   // Define o pino do sensor DHT22
#define DHTTYPE DHT11   // Define o tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// LEDs config
#define LEDtemp 15
#define LEDhumi 18
#define LEDok   23
#define LEDhigh 22

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
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void check_temp_humi(float unid, int lim){
  if (isnan(unid)){ // FALHA NA LEITURA DE DADOS
    Serial.print("falha ao ler dados do sensor DHT11");
    return;
  }
  else if (unid <= lim){ // TEMP ou HUMI OK
    digitalWrite(LEDok, HIGH);
    digitalWrite(LEDhigh, LOW);
  }
  else{
    digitalWrite(LEDok, LOW);
    digitalWrite(LEDhigh, HIGH);
  }
}

void getTempPage(AsyncWebServerRequest *request) {
  float temp = dht.readTemperature();
  check_temp_humi(temp, lim_temp);
  digitalWrite(LEDtemp, HIGH);
  digitalWrite(LEDhumi, LOW);
  Serial.print("temp: ");
  Serial.print(temp);
  Serial.println("°C ");
  request->send(SPIFFS, "/temp.html", "text/html");
}

void handleTemp(AsyncWebServerRequest *request) {
  float temp = dht.readTemperature();   // Lê a temperatura do sensor
  String TempValue = String(temp);
  request->send(200, "text/plane", TempValue); //Send temp value only to client ajax request
}



void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LEDtemp    , OUTPUT);
  pinMode(LEDhumi    , OUTPUT);
  pinMode(LEDok      , OUTPUT);
  pinMode(LEDhigh    , OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LEDtemp    , LOW);
  digitalWrite(LEDhumi    , LOW);
  digitalWrite(LEDok      , LOW);
  digitalWrite(LEDhumi    , LOW);
  digitalWrite(LED_BUILTIN, LOW);

  initFS();
  initWiFi();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    digitalWrite(LEDtemp, LOW);
    digitalWrite(LEDhumi, LOW);
    digitalWrite(LEDok,   LOW);
    digitalWrite(LEDhumi, LOW);
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    digitalWrite(LEDtemp, LOW);
    digitalWrite(LEDhumi, LOW);
    digitalWrite(LEDok,   LOW);
    digitalWrite(LEDhumi, LOW);
  });

  server.on("/help.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/help.html", "text/html");
    digitalWrite(LEDtemp, LOW);
    digitalWrite(LEDhumi, LOW);
    digitalWrite(LEDok,   LOW);
    digitalWrite(LEDhumi, LOW);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");});
  
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/js");});

  server.on("/navbar.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/navbar.html", "text/html");});
  
  server.on("/footer.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/footer.html", "text/html");});

  /*
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/temp.html", "text/html");
  });
  
  server.on("/temp", HTTP_GET, handleTemp);
  ------

  server.on("/temp.html", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/temp.html", "text/html");
  });
  */
  
  server.on("/temp.html", HTTP_GET, getTempPage); //To get update of temp value only
  server.on("/temp/getTemp", HTTP_GET, handleTemp);


  server.serveStatic("/", SPIFFS, "/");
  
  server.begin(); //Start server
}

void loop() {
}