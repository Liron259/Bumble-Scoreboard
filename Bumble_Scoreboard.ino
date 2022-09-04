#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

char openChar = 31, closeChar = 9;
const String home_score = "0W10";
const String guest_score = "0W20";
const String home_foul = "0W40";
const String guest_foul = "0W50";
const String left_arrow = "0C1";
const String right_arrow = "0C2";
const String run_clock_up = "0T1";
const String run_clock_down = "0T2";
const String horn = "0P0";


const char* ssid = "Scoreboard";
const char* password = "Scoreboard";

int counter = 0;


SoftwareSerial scoreboard(13, 2);
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  scoreboard.begin(9600);
  
  // prepare LED
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(0, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, 1);

  WiFi.softAP(ssid, password);

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.softAPIP());
}

void loop() {
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("new client");

  client.setTimeout(100); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println("request: ");
  Serial.println(req);

  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }
  // Match the request
  int val;
  if(req.indexOf("/gpio/") != -1 || req.equals("") || req.indexOf("/time")  != -1|| req.indexOf("/arrow/")  != -1|| req.indexOf("/counter/")  != -1|| req.indexOf("?") != -1){
    if (req.indexOf("/gpio/0") != -1) {
      val = 1;
    } else if (req.indexOf("/gpio/1") != -1) {
      val = 0;
    } else if (req.indexOf("/gpio/-") != -1) {
      counter--;
      updateScore(counter);
    } else if (req.indexOf("/gpio/+") != -1) {
      counter++;
      updateScore(counter);
    } else if (req.indexOf("/timer/") != -1 || req.indexOf("?timer=") != -1) {
      int index = req.indexOf("?") != -1 ? req.indexOf("?") : req.indexOf("/");
      int offset = 7;
      String inputTime = req.substring(index + offset, req.indexOf(" ", index + offset));
      Serial.println(inputTime);
      startTimer(inputTime);      
    } else if (req.indexOf("/arrow/") != -1) {
      String dir = req.substring(req.indexOf("/arrow/") + 7, req.indexOf(" ", req.indexOf("/arrow/") + 7));
      showArrow(dir);      
    } else if (req.indexOf("/counter/") != -1) {
      counter = req.substring(req.indexOf("/counter/") + 9, req.indexOf(" ", req.indexOf("/counter/") + 9)).toInt();
      updateScore(counter);
    }
  
    // Set LED according to the request
    digitalWrite(LED_BUILTIN, val);
  
    // read/ignore the rest of the request
    // do not client.flush(): it is for output only, see below
    
  
    // Send the response to the client
    // it is OK for multiple small client.print/write,
    // because nagle algorithm will group them into one single packet
    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/html\r\n\r\n");
    client.print("<!DOCTYPE HTML>\r\n<html>\r\n");
    client.print("GPIO is now ");
    client.print(val ? "low" : "high");
    client.print("<br><br>Click <a href='/gpio/1'>here</a> to switch LED GPIO on, or <a href='/gpio/0'>here</a> to switch LED GPIO off.");
    client.print("<br><br><a href='/gpio/-'><button type='button'>-</button></a>&nbsp");
    client.print(counter);
    client.print("&nbsp <a href=\"/gpio/+\"><button type='button'>+</button></a>");

    client.print("<br><br><a href=\"/arrow/left\"><button type='button'><</button></a>");
    client.print("&nbsp <a href=\"/arrow/right\"><button type='button'>></button></a>");
    client.print("<br><br> <form action:\"/get\">time: <input type=\"text\" name=\"timer\"><input type=\"submit\" value=\"Start\"></form>");
    client.print("</html>");
  }else if(req.indexOf(F("/counter")) != -1){
    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/html\r\n\r\n");
    client.print("<!DOCTYPE HTML>\r\n<html>\r\n");
    client.print(counter);
    client.print("</html>");
  }

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}

void updateScore(int counter) {
  String output = String(counter);
  if (counter < 10) {
    output = "0" + String(counter);
  }
  updateScreen(home_score + output);
  output = String(counter * 2);
  if (counter * 2 < 10) {
    output = "0" + String(counter * 2);
  }
  updateScreen(guest_score + output);
}

void startTimer(String timeout){
  String t = timeout.substring(0, 2) + timeout.substring(3, 5) + timeout.substring(6, 8);
  updateScreen(run_clock_down + t);
}

void showArrow(String dir){
  if(dir == "left"){
    updateScreen(left_arrow);
  }else if(dir == "right"){
    updateScreen(right_arrow);
  }
}

void updateScreen(String command) {
  scoreboard.print(((char) openChar) + command + ((char) closeChar));
  scoreboard.print(((char) openChar) + command + ((char) closeChar));
  Serial.println(command);
}
