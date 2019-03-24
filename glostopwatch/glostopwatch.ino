/*
    
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#define TODO 0
#define DONE 1

const char* ssid = "PdG 3";
const char* password = "niquez_vous";

const char* host = "gloapi.gitkraken.com";
const int httpsPort = 443;


const char *bearer = "Bearer p40fff5ab0c94f26641daddfe014cf0c2c28b346b";

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";

//Board ID's Todo list

WiFiClientSecure client;
  

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

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


  String url_boards = "/v1/glo/boards";

  String line = GloGet(url_boards);
  
  Serial.println(line);
  if(line != NULL){
    const char * board_id;
      //Limited to 10 boards
     const int capacity = JSON_OBJECT_SIZE(2)*10;
     DynamicJsonDocument doc(capacity);
     deserializeJson(doc, line);
     const char* name = doc[0]["name"];
     Serial.println(name);
     name = doc[1]["name"];
     Serial.println(name);
     name = doc[2]["name"];
     Serial.println(name);
     board_id = doc[0]["id"];
     String url_columns = "/v1/glo/boards/";
     url_columns += "?board_id=";
     url_columns += board_id;
     Serial.print(url_columns);
     String newLine = GloGet(url_columns);
     if(newLine != NULL){
        Serial.println("lol");
     }
  }
  
}

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
}
