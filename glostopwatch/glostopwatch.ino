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

#define MAX_LENGTH 3

struct board
{
  String name;
  String id ;
  bool selected;
};
typedef struct board Board;

struct card
{
  String name;
  String id ;
  bool selected;
};
typedef struct card Card;


// Graph the rssi of this wifi
const char* ssid = "SFR_A930";
const char* password = "bzs82agse4iuc53bvkp9";

//const char* ssid = "PdG 3";
//const char* password = "niquez_vous";


const char* host = "gloapi.gitkraken.com";
const int httpsPort = 443;


const char *bearer = "Bearer p40fff5ab0c94f26641daddfe014cf0c2c28b346b";

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";

WiFiClientSecure client;

const unsigned char GlogoBitmap [] PROGMEM = {
  // 'logo, 84x48px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x70, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0x81, 0xf8, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0xfc, 0x07, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x07, 0xfe, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x0f, 0xff, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x1f, 0xff, 0x80, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3f, 0xff, 0xc0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xe0, 0x3f, 0xff, 0xc0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x7f, 0xff, 0xe0,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x80, 0xff, 0xff, 0xf0, 0x18, 0x80, 0x00, 0x00, 0x00,
  0x00, 0x39, 0x80, 0xff, 0xff, 0xf0, 0x1c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x33, 0x01, 0xff, 0xff,
  0xf8, 0x0c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x33, 0x01, 0xff, 0xff, 0xf8, 0x0c, 0xc0, 0x00, 0x00,
  0x00, 0x00, 0x33, 0x01, 0xff, 0xff, 0xf8, 0x0e, 0x40, 0x00, 0x00, 0x00, 0x00, 0x26, 0x03, 0xff,
  0xff, 0xfc, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x66, 0x03, 0xff, 0xff, 0xfc, 0x06, 0x60, 0x00,
  0x00, 0x00, 0x00, 0x66, 0x01, 0xff, 0xff, 0xf8, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00,
  0xff, 0xff, 0xf0, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x7f, 0xff, 0xe0, 0x06, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x3f, 0xff, 0xe0, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x66,
  0x00, 0x3f, 0xff, 0xc0, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x3f, 0xff, 0xc0, 0x06,
  0x60, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x3f, 0xff, 0xc0, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x37, 0x00, 0x3f, 0xff, 0xc0, 0x0e, 0x40, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x43, 0xfe, 0x20,
  0x0c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x43, 0xfc, 0x20, 0x0c, 0xc0, 0x00, 0x00, 0x00,
  0x00, 0x3b, 0x80, 0x43, 0xfc, 0x20, 0x0c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x19, 0x80, 0x27, 0xfe,
  0x40, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x19, 0xc0, 0x1f, 0xff, 0x80, 0x19, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x1c, 0xc0, 0x1f, 0xff, 0x80, 0x33, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x60, 0x1b,
  0xfd, 0x80, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x70, 0x19, 0x99, 0x80, 0xe7, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x07, 0x38, 0x19, 0x99, 0x81, 0xce, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x9c,
  0x19, 0x99, 0x83, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x8e, 0x19, 0x99, 0x87, 0x18, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x99, 0x99, 0x9e, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf1, 0xf9, 0x99, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0xf9, 0x99, 0xf1, 0xc0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x39, 0x99, 0xc3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0f, 0x01, 0x98, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe1, 0x98, 0x7c,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0x9f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x1f, 0x9f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x98,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



//LCD pins
const int8_t RST_PIN = D2;
const int8_t CE_PIN = D1;
const int8_t DC_PIN = D6;
const int8_t BL_PIN = D0;

//Buttons pins
const int8_t BTN_LEFT = D3;
const int8_t BTN_RIGHT = D4;
//Buttons variables
bool leftClick = false;
bool rightClick = false;

//Display object
Adafruit_PCD8544 display = Adafruit_PCD8544(DC_PIN, CE_PIN, RST_PIN);

//Board list
Board* boards = NULL;
int boardCurr = 0;
int boardPrev = 0;
int boardSize = 3;
bool boardMode = false;

//Column
String column_id;
Card* cards = NULL;
int cardCurr = 0;
int cardPrev = 0;
bool cardMode = false;

void ConnectToWiFi() {
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
  display.print("Connected !");
  display.display();
  delay(500);
}

void ConnectToGloAPI() {

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

void InitLCD() {
  // Turn LCD backlight on
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);

  // Configure LCD
  display.begin();
  display.setContrast(60);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.clearDisplay();
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  
  InitLCD();

  DisplayGloLogo();
  //Some time for the logo to be displayed
  delay(2000);

  ConnectToWiFi();

  ConnectToGloAPI();

  UpdateBoards();
  
  DisplayBoards();
}

void UpdateBoards() {
  display.clearDisplay();
  display.print("Updating your boards");
  display.display();
  String url_boards = "/v1/glo/boards";
  String line = GloGet(url_boards);

  //free(boards);
  boards = (Board*)calloc(boardSize,sizeof(Board));
  Serial.println("Fetching your boards!");
  if (line != NULL) {
    //Limited to 10 boards
    const int capacity = JSON_OBJECT_SIZE(2) * 10;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, line);
    const char* buff;
    for (int i = 0; i < boardSize; i++) {
      buff = doc[i]["name"];
      boards[i].name = String(buff);
      buff = doc[i]["id"];
      boards[i].id = String(buff);
      boards[i].selected = false;
    }
    //Automaticaly selects the 1rst one
    boards[boardCurr].selected = true;

  }
  boardMode = true;
}

void UpdateCards() {
  display.clearDisplay();
  display.print("Fetching your cards");
  display.display();
  String board_id = boards[boardCurr].id;
  String url_columns = "/v1/glo/boards/";
  url_columns += board_id;
  url_columns += "/columns/";
  url_columns += column_id;
  url_columns += "/cards";
  
  String line = GloGet(url_columns);
  //free(cards);
  cards = (Card*)calloc(boardSize,sizeof(Card));
  Serial.println("Fetching your cards!");
  if (line != NULL) {
    //Limited to 10 boards
    const int capacity = JSON_OBJECT_SIZE(4) * 10;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, line);
    const char* buff;
    for (int i = 0; i < boardSize; i++) {
      buff = doc[i]["name"];
      cards[i].name = String(buff);
      buff = doc[i]["id"];
      cards[i].id = String(buff);
      cards[i].selected = false;
    }
    //Automaticaly selects the 1rst one
    cards[cardCurr].selected = true;

  }
  cardMode = true;
  DisplayCards();
}


void DisplayBoards() {
  //int arraySize = sizeof(boards) / sizeof(boards[0]);
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.print("    BOARDS    ");
  for (int i = 0; i < boardSize; i++) {  
    Board bd = boards[i];
    if(bd.selected == true){
      display.setTextColor(WHITE,BLACK);
    }else{
      display.setTextColor(BLACK);
    }
    
    display.print(bd.name);
    display.print("\n");
    Serial.println(bd.name);
    Serial.println(bd.id);
  }
  display.display();
}

void DisplayCards() {
  //int arraySize = sizeof(boards) / sizeof(boards[0]);
  String title = boards[boardCurr].name;
  title = title.substring(0,14);
  title.toUpperCase();
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.print(title);
  for (int i = 0; i < boardSize; i++) {  
    Card cd = cards[i];
    if(cd.selected == true){
      display.setTextColor(WHITE,BLACK);
    }else{
      display.setTextColor(BLACK);
    }
    
    display.print(cd.name);
    display.print("\n");
    Serial.println(cd.name);
  }
  display.display();
}


void DisplayGloLogo() {
  display.clearDisplay();
  // Display bitmap
  display.drawBitmap(0, 0, GlogoBitmap, 84, 48, BLACK);
  display.display();
  display.clearDisplay();
}

//returns a JSON string from the GET method
String GloGet(String url) {

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: " + bearer + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: keep-alive\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println(line);
  return line;
}

void CheckButtons(){
  int left = digitalRead(BTN_LEFT);
  int right = digitalRead(BTN_RIGHT);
   if (left == LOW){
    leftClick = true;
   }
   else{
    leftClick = false;
   }
   if (right == LOW){
    rightClick = true;
   }
   else{
    rightClick = false;
   }
}

void HandleBoardsInput(){
    if(leftClick){
      boardPrev = boardCurr;
      boardCurr++;
      boardCurr = boardCurr%boardSize;
    }
    if(rightClick){
      //Validate
       boardMode = false;
       //UpdateBoards();
       GetBoardColumnId();
       UpdateCards();
    }
}

void GetBoardColumnId(){
  
  display.clearDisplay();
  display.print("Fetching your columnd id");
  display.display();
  String board_id = boards[boardCurr].id;
  
  String url_columns = "/v1/glo/boards/";
  url_columns += board_id;
  url_columns += "?fields=columns";
  Serial.println(url_columns);
  String line = GloGet(url_columns);
  Serial.println("Recieved:");
  Serial.println(line);
  if (line != NULL) {
    const int capacity = JSON_ARRAY_SIZE(4) + 4*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, line);
    JsonArray columns = doc["columns"];
    JsonObject columns_0 = columns[0];
    const char* buff = columns_0["id"];
    column_id = String(buff);
    buff = columns_0["name"];
    Serial.println(buff);
  }
}

void HandleCardsInput(){
    if(leftClick){
      cardPrev = cardCurr;
      cardCurr++;
      cardCurr = cardCurr%boardSize;
    }
    if(rightClick){
      //Validate
       cardMode = false;
       //UpdateBoards();
       //GetBoardColumnId();
       //UpdateCards();
      Serial.println("You picked a card!");
    }
}

void HandleInputs(){
  if(boardMode == true){
    HandleBoardsInput();
    return;
  }
  if(cardMode == true){
    HandleCardsInput();
    return;
  }
  else{
    //In Card mode, counting time & stuff
  }
}






void UpdateDisplay(){
  if(boardMode == true){
    UpdateBoardsDisplay();
    return;
  }
  if(cardMode == true){
    UpdateCardsDisplay();
    return;
  }
  else{
    //In Card mode, counting time & stuff
  }
}

void UpdateBoardsDisplay(){
    if(boardPrev != boardCurr){
      for(int i = 0; i < boardSize; i++){
        if(i == boardCurr){
          boards[i].selected = true;
        }else{
          boards[i].selected = false;
        }
      }
      DisplayBoards();
    }
}

void UpdateCardsDisplay(){
  if(cardPrev != cardCurr){
      for(int i = 0; i < boardSize; i++){
        if(i == cardCurr){
          cards[i].selected = true;
        }else{
          cards[i].selected = false;
        }
      }
      DisplayCards();
    }
}

void loop() {
  CheckButtons();
  HandleInputs();
  UpdateDisplay();
  delay(100);  // Adjust this to change graph speed
}
