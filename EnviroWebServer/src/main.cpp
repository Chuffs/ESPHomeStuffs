#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <ArduinoJson.h>
#include <AsyncJson.h>

#define DHTPIN 12
#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);
double ESPtemp = 0; double ESPhumid = 0;
char ESPtemp_str[10]; char ESPhumid_str[10];

AsyncWebServer server(80);
DynamicJsonDocument doc(1024);

const char* APSSID = "ESP";
IPAddress staticIP(192, 168 ,0, 4);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void WiFiStart() {
  Serial.print("Setting AP Config: ");
  Serial.println(WiFi.softAPConfig(staticIP, gateway, subnet) ? "Ready!" : "Failed!");

  Serial.print("Setting AP SSID: ");
  Serial.println(WiFi.softAP(APSSID) ? "Ready" : "Failed!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "Home");
  });

  server.on("/get_update", HTTP_GET, [](AsyncWebServerRequest *request){
    String response_str;
    serializeJson(doc, response_str);
    request->send(200, "text/html", response_str);
  });
 
  server.onNotFound(notFound);
  server.begin();
}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Serial Ready!");

  dht.begin();

  WiFiStart();
}

void loop() {
  sensors_event_t event;

  delay(500);

  dht.temperature().getEvent(&event);
  doc["temperature"] = event.temperature;

  dht.humidity().getEvent(&event);
  doc["humidity"] = event.relative_humidity;

  Serial.print(F("Temperature: "));
  Serial.print(ESPtemp);
  Serial.println(F("°C"));
  Serial.print(F("Relative Humidity: "));
  Serial.print(ESPhumid);
  Serial.println(F("%"));
}