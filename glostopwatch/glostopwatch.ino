/*
    Glo Stopwatch
    
    MIT License
    By Antoine CHEREL
    

    Connections:
    WeMos D1 Mini   Nokia 5110    Description
    (ESP8266)       PCD8544 LCD
    
    D2 (GPIO4)      0 RST         Output from ESP to reset display
    D1 (GPIO5)      1 CE          Output from ESP to chip select/enable display
    D6 (GPIO12)     2 DC          Output from display data/command to ESP
    D7 (GPIO13)     3 Din         Output from ESP SPI MOSI to display data input
    D5 (GPIO14)     4 Clk         Output from ESP SPI clock
    3V3             5 Vcc         3.3V from ESP to display
    D0 (GPIO16)     6 BL          3.3V to turn backlight on, or PWM
    G               7 Gnd         Ground

*/

#include <Arduino.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#define TODO 0
#define DONE 1

// Graph the rssi of this wifi
const char* ssid = "SFR_A930";
const char* password = "bzs82agse4iuc53bvkp9";

const char* host = "gloapi.gitkraken.com";
const int httpsPort = 443;


const char *bearer = "Bearer p40fff5ab0c94f26641daddfe014cf0c2c28b346b";

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";

WiFiClientSecure client;


//LCD pins
const int8_t RST_PIN = D2;
const int8_t CE_PIN = D1;
const int8_t DC_PIN = D6;
const int8_t BL_PIN = D0;

Adafruit_PCD8544 display = Adafruit_PCD8544(DC_PIN, CE_PIN, RST_PIN);

 

void ConnectToWiFi(){
  Serial.print("connecting to ");
  display.print("connecting to ");
  Serial.println(ssid);
  display.print(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    //updates the display
    display.display();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.print("connected");
  display.display();
  delay(500);
}

void ConnectToGloAPI(){
  
  // Use WiFiClientSecure class to create TLS connection
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

}

void InitLCD(){
  // Turn LCD backlight on
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);

  // Configure LCD
  display.begin();
  display.setContrast(60);  
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.clearDisplay();
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  InitLCD();
  
  ConnectToWiFi();
  
  ConnectToGloAPI();
  
  String url_boards = "/v1/glo/boards";
  String line = GloGet(url_boards);
  Serial.println(line);
  if(line != NULL){
    const char * board_id;
    const int gloBoardCount = 3;
      //Limited to 10 boards
     const int capacity = JSON_OBJECT_SIZE(2)*10;
     DynamicJsonDocument doc(capacity);
     deserializeJson(doc, line);
     const char* boardName;
     display.clearDisplay();
     for(int i = 0; i< gloBoardCount; i++){
      boardName = doc[i]["name"];
      display.print(boardName);
      display.print("\n");
      Serial.println(boardName);
     }
     display.display();
     board_id = doc[0]["id"];
     String url_columns = "/v1/glo/boards/";
     url_columns += "?board_id=";
     url_columns += board_id;
     Serial.print(url_columns);
     
  }
  
}


//returns a JSON string from the GET method
String GloGet(String url){
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: " + bearer + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  return line;
}


void loop() {
  display.clearDisplay();
  display.printf("IN THE MAIN LOOP \n");


  delay(100);  // Adjust this to change graph speed
}
