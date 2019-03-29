# Glo  Board Time Tracker
Arduino code to make a Glo Board time manager device! You can find a tutorial on how to make such device [here](https://www.instructables.com/id/Connected-Stopwatch/).
This lets you use physical buttons to start,pause and end a task. The card will automatically get a comment with the time you needed to complete it.
This code is developed for the ESP8266 board, but can be adapted to suit any Arduino compatible device. 

## Dependencies:

Add these to your Arduino IDE using Manage Libraries.
* [Arduino Json](https://github.com/bblanchon/ArduinoJson)
* [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
* [Adafruit PCD8544 Nokia 5110 LCD library](https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library)
  * There is a pull request that adds ESP8266 support:
  * [adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library#27](https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library/pull/27)
  * If this has not been merged yet, you will need to manually copy the changes across
 
## Pins:

| ESP8266 WeMos D1 Mini | Nokia 5110 LCD
| ------------- | ---------------------- | 
| D2 (GPIO4)    | 0 RST         
| D1 (GPIO5)    | 1 CE                   
| D6 (GPIO12)   | 2 DC                   
| D7 (GPIO13)   | 3 DIN                 
| D5 (GPIO14)   | 4 CLK                                  
| 3V3           | 5 VCC                                    
| D0 (GPIO16)   | 6 BL                            
| G (GND)       | 7 GND   
 
| ESP8266 WeMos D1 Mini | Buttons                    
| ------------- | ---------------------- | 
| D3 (GPI18)    | Left Button         
| D4 (GPI17)    | Right Button                    
