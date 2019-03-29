/*
    Glo Stopwatch

    MIT License
    By Antoine CHEREL
    https://github.com/antoinech/glo-stopwatch

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

    D3              Button Left
    D4              Right Button
    (the other button's pin needs to be tied ground)

*/

#include <Arduino.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#define MAX_LENGTH 10

//Struct used to create lists of Cards/Boards
struct item
{
  String name;
  String id;
  bool selected;
};
typedef struct item Item;


//Put your WiFi credentials here
const char* ssid = "--your--ssid--";
const char* password = "--your--password--";

WiFiClientSecure client;
const char* host = "gloapi.gitkraken.com";
const int httpsPort = 443;

//Put your Personal Access Token (https://support.gitkraken.com/developers/pats/)
const char *bearer = "Bearer XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

// Use web browser to view and copy
// SHA1 fingerprint of the certificate from https://gloapi.gitkraken.com/v1/glo
const char* fingerprint = "6A A4 FE D7 4F 42 8F 0E 34 CD 23 DF 3C 73 E2 D1 F5 84 19 73";



//Glo logo, 84x48px
const unsigned char GlogoBitmap [] PROGMEM = {  
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

//Display object for the Nokia LCD screen
Adafruit_PCD8544 display = Adafruit_PCD8544(DC_PIN, CE_PIN, RST_PIN);

//Board list
Item* boards = NULL;
int boardCurr = 0;
int boardPrev = 0;
int boardSize = 0;
bool boardMode = false;

//Column
String column_id;
Item* cards = NULL;
int cardCurr = 0;
int cardPrev = 0;
int columnSize = 0;
bool cardMode = false;

//Stopwatch
String taskName;
bool taskPlaying = false;
bool taskDone = false;

//Time in milliseconds
unsigned long elapsedTime = 0;
unsigned long lastTime;

//Connects to the provided SSID
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

//Connects to the Glo API with the provided host url
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

//Initializes the display
void InitLCD() {
  // Turn LCD backlight on
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);

  // Configure LCD
  display.begin();
  display.setContrast(50);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.clearDisplay();
}

//Setup Loop (called on boot)
void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  
  InitLCD();
  
  DisplayGloLogo();
  delay(2000);

  ConnectToWiFi();

  ConnectToGloAPI();

  UpdateBoards();
  
  DisplayBoards();
}

/********** GET *********/

//Makes a Get request on the Glo API and returns the result as string
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



//Fetchs the users's board thanks to its Personnal Access Token
void UpdateBoards() {
  display.clearDisplay();
  display.print("Updating your boards");
  display.display();
  String url_boards = "/v1/glo/boards";
  String line = GloGet(url_boards);

  Serial.println("Loading your boards!");
  if (line != NULL) {
    //Limited to 10 boards
    const int capacity = JSON_OBJECT_SIZE(2) * 10;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, line);
    
    boardSize = doc.size();

    free(boards);
    boards = (Item*)calloc(boardSize,sizeof(Item));  
    
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

//Updates the card list from the 1rst column of the previously selected board
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
  Serial.println("Fetching your cards!");
  if (line != NULL) {
    //Limited to 10 boards
    const int capacity = JSON_OBJECT_SIZE(4) * MAX_LENGTH;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, line);
    
    columnSize = doc.size();
    free(cards);
    cards = (Item*)calloc(columnSize,sizeof(Item));
    
    const char* buff;
    for (int i = 0; i < columnSize; i++) {
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

//Called to get the Board's ID when selected
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
    const char* buff = ((JsonObject)columns[0])["id"];
    column_id = String(buff);
  }
}

/*********** POST *************/

//Called when a task/card is done, to comment the time you spent
void PostTimeComment(){
  display.clearDisplay();
  display.print("Card Done !");
  display.display();
  String board_id = boards[boardCurr].id;
  String card_id = cards[cardCurr].id;
  String url_comment = "/v1/glo/boards/";
  url_comment += board_id;
  url_comment += "/cards/";
  url_comment += card_id;
  url_comment += "/comments";
  Serial.print(url_comment);

  unsigned long Now = elapsedTime/1000;
  int Seconds = Now%60;
  int Minutes = (Now/60)%60;
  int Hours = (Now/3600)%24;
  
  String message = "You finished your card in ";
  if(Hours != 0){
    message += String(Hours) + " hours, ";
  }
  if(Minutes != 0){
    String m = intToDigit(Minutes);
    message += m + " minutes and ";
  }
  String s = intToDigit(Seconds);
  message += s + " seconds";
  
  
  String PostData = "{";
  PostData +="\"text\":\""+ message +"\"";
  PostData += "}";
  
  Serial.print(PostData);
  
  client.print(String("POST ") + url_comment + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: " + bearer + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "cache-control: no-cache\r\n" +
               "Content-Type: application/json \r\n" +
               "Content-Length: " + PostData.length() + "\r\n" +
               "\r\n" + PostData + "\n");
               
  Serial.println("request sent");
}


/************ Registering Button Presses **********/

//Registers the inputs on the buttons
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




/************ Using the Inputs ***************/

//Depending on the steps in the menu, calls the according Input Handler function
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
    HandleTaskInput();
  }
}

//Board Menu Inputs
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

//Card Menu Inputs
void HandleCardsInput(){
    if(leftClick){
      cardPrev = cardCurr;
      cardCurr++;
      cardCurr = cardCurr%columnSize;
    }
    if(rightClick){
      //You picked your task!
       cardMode = false;
       taskName = cards[cardCurr].name;
       taskPlaying = true;
       lastTime = millis();
       //avoid ending the card when selecting a card
       delay(100);
    }
}

//Card Play/Pause/Done Menu Inputs
void HandleTaskInput(){
   if(leftClick){
      //Play/Pause button
      taskPlaying = !taskPlaying;
   }
   if(rightClick){
      //stop the counter
      taskPlaying = false;
      //Done with the task!
      taskDone = true;
      PostTimeComment();
      boardMode = false;
      cardMode = false;
      UpdateBoards();
   }
}




/************ Displaying Screens ***************/

//Main Display Function
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
    UpdateTaskDisplay();
  }
}

//Board Menu Display 
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

//Card Menu Display
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

//Card Play/Pause/Done Menu Display  
void UpdateTaskDisplay(){
   display.clearDisplay();
   display.setTextColor(BLACK);
   //Symbol (Play/Pause/Done)
   if(taskDone){
     display.write(219);
   }
   else if(!taskPlaying){
     display.write(185);
   }else{
     display.write(16);
   }
   display.print(" ");
   
   display.print(taskName);
   
   display.setTextSize(2);
   display.setCursor(0,32);
   PrintTime();
   display.setTextSize(1);
   display.display();
}

//Display function for Boards
void DisplayBoards() {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.print("    BOARDS    ");
  for (int i = 0; i < boardSize; i++) {  
    Item bd = boards[i];
    if(bd.selected == true){
      display.setTextColor(WHITE,BLACK);
    }else{
      display.setTextColor(BLACK);
    }
    Serial.print(bd.name);
    display.print(bd.name);
    display.print("\n");
  }
  display.setTextColor(BLACK);
  display.display();
}

//Display function for Cards
void DisplayCards() {  
  display.clearDisplay();
  //DISPLAYING THE BOARD TITLE
  String title = boards[boardCurr].name;
  title = title.substring(0,14);
  title.toUpperCase();
  int l = title.length();
  int pixels = 6 * l;
  
  //if the card title is smaller than the 84pixels composing the screen
  if(pixels < 84){
    int padding = int((84 - pixels)/2);
    display.setCursor(padding,0);
  }
  display.setTextColor(BLACK);
  display.print(title + "\n");
  
  for (int i = 0; i < columnSize; i++) {  
    Item cd = cards[i];
    if(cd.selected == true){
      display.setTextColor(WHITE,BLACK);
    }else{
      display.setTextColor(BLACK);
    }
    
    display.print(cd.name);
    display.print("\n");
  }
  display.setTextColor(BLACK);
  display.display();
}



//Displays the Glo logo turned into bytes
void DisplayGloLogo() {
  display.clearDisplay();
  // Display bitmap
  display.drawBitmap(0, 0, GlogoBitmap, 84, 48, BLACK);
  display.display();
  display.clearDisplay();
}


/************* Time Tracking *************/

void PrintTime(){
  //Time in seconds
  unsigned long Now = elapsedTime/1000;
  int Seconds = Now%60;
  int Minutes = (Now/60)%60;
  int Hours = (Now/3600)%24;
  String s = intToDigit(Seconds);
  String m = intToDigit(Minutes);
  display.print(String(Hours));
  display.print(":");
  display.print(m);
  display.print(":");
  display.print(s);
}

//Turns seconds/minutes/ into digital clock numbers (ex: 01:07, instead of 1:7)
String intToDigit(int val){
  String out;
  if(val > 9){
    out = String(val);
  }else{
    out = "0" + String(val);
  }
  return out;
}


//Counts the time spent between 2 loops
void UpdateTime(){
  unsigned long now = millis();
  if(taskPlaying){
    elapsedTime += now - lastTime;  
  }
  lastTime = now;
}

/******* MAIN LOOP *************/

void loop() {
  CheckButtons();
  HandleInputs();
  UpdateTime();
  UpdateDisplay();
  delay(100);  // Adjust this to change screed refresh rate
}
