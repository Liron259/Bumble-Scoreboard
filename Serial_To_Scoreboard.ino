#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#ifndef STASSID
#define STASSID "Scoreboard"
#define STAPSK  "Scoreboard"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* url = "http://192.168.4.1/counter/";

SoftwareSerial arduino(2, 13);

void setup() {
  Serial.begin(115200);
  arduino.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
}

void loop() {
  if(arduino.available() > 0){
    static char message[12];
    static unsigned int message_pos = 0;
    char inByte = arduino.read();
    if(inByte != '\n' && (message_pos < 11)){
      message[message_pos] = inByte;
      message_pos++;
    }else{
      message[message_pos] = '\0';
      message_pos = 0;
      uploadToArduino(message);
    }
  }
}

void uploadToArduino(String counter){
  String uri = url + counter;
  WiFiClient client;
  HTTPClient http;
  Serial.println(uri);
  http.begin(client, uri);
  http.GET();
  http.end();
}
