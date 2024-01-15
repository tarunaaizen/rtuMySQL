#include <SimpleModbusSlave.h>

#ifdef RISC
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <riscWiFi.h>
  #include <riscHTTPClient.h>
  #include <WiFiClient.h>
#endif

#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "fallOUTrj2";
const char* password = "fallINULj2";

const char* serverName = "http://taruna.com/passnumber.php";

String apiKeyValue = "PmAT5A@%52b3j7F9tigabelas#";


String sensorName = "DHT22";
String sensorLocation = "Office";

enum 
{     
  T_DHT22,
  H_DHT22, 
  I_DHT22,   
  HOLDING_REGS_SIZE
};

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array

long durasiScada = 0;
long jedaScada = 20;
long durasiIot = 0;
long jedaIot = 5000;

void setup() {

  Serial2.begin(38400);
  modbus_configure(&Serial2, 38400, SERIAL_8N2, 1, 2, HOLDING_REGS_SIZE, holdingRegs);
  modbus_update_comms(38400, SERIAL_8N2, 7);
  
  dht.begin();
  WiFi.begin(ssid, password);
}

void loop() {


  if (millis()- durasiScada > jedaScada){

  modbus_update();
   float h = dht.readHumidity();
   float t = dht.readTemperature();
   float f = dht.readTemperature(true);
   float hif = dht.computeHeatIndex(f, h);
   float hic = dht.computeHeatIndex(t, h, false);
   
  holdingRegs[T_DHT22] = dht.readTemperature();                                                       
  holdingRegs[H_DHT22] = dht.readHumidity();
  holdingRegs[I_DHT22] = dht.computeHeatIndex(t, h, false);
  
  durasiScada = millis();
  }


if (millis()- durasiIot > jedaIot){
    
    WiFiClient client;
    HTTPClient http;

   float h = dht.readHumidity();
   float t = dht.readTemperature();
   float f = dht.readTemperature(true);
   float hif = dht.computeHeatIndex(f, h);
   float hic = dht.computeHeatIndex(t, h, false);

    http.begin(client, serverName);    
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");    
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                          + "&location=" + sensorLocation + "&value1=" + String(t)
                          + "&value2=" + String(h) + "&value3=" + String(hic) + "";
    int httpResponseCode = http.POST(httpRequestData);
    
    durasiIot = millis();
    }       
}