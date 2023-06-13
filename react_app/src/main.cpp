#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "FS.h"
#include <SPIFFS.h>
#include <Update.h>

const char* ssid = "Lau";          //SSID
const char* password = "12345678"; //Password

// DHT config
#define DHTPIN 19   // Define o pino do sensor DHT22
#define DHTTYPE DHT11   // Define o tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

String TempS;
String HumS;
int block=1; //Variable para iniciar comunicación y enviar datos por plotly.js
String mensaje;

using namespace websockets;
WebsocketsClient client; //Crear un cliente para websocket-
WebsocketsServer server;
void setup() {
  Serial.begin(115200);
  dht.begin();
  //Conectar a Wifi
  WiFi.begin(ssid, password);

  //Espera a que se conecte
  for(int i = 0; i < 100 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);
  }
  //Mostrar IP asignada
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  server.listen(8003); //Servidor en puerto 8003
  Serial.print("Servidor Activo?");
  Serial.println(server.available());
  
}

void loop() {
  float t = dht.readTemperature(); //Leer temperatura
  float h = dht.readHumidity(); //Leer humedad
  //Imprimir en consola temperatura y humedad
  Serial.print("Humidity(%RH): ");
  Serial.println(h);
  Serial.print(" Temperature(C): ");
  Serial.println(t);
  TempS=String(t);//Convertir temperatura en String para enviarla por websocket
  HumS=String(h); //Convertir humedad en String para enviarla por websocket
  if(block==1){//Si dato leido es 1 entonces crear la conexión y generar recepción continua
    client = server.accept();
    client.poll();
    block=2;
  }
  
  if(client.available()) {
    auto msg = client.readBlocking();
    Serial.print("Leyendo...");
    Serial.println(msg.data());//Imprimiendo data leido desde página con plotly (Cliente)
    client.send(TempS+","+HumS);
    //Cerrar la conexión si dato leido es 2 (comando)
    if(msg.data()=="2"){
      client.close();
    }
  }
  delay(50);//Retraso de 50ms
}