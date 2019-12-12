/*
  Program written by JelleWho, based on DIYHUE ESP8266
  Board: https://dl.espressif.com/dl/package_esp32_index.json


  IFTT SETTINGS:
  http://77.173.36.55/  POST  application/x-www-form-urlencoded   A?light=5
  ->  Method: 2 URI: /    arg 'A?light' = '5'

  http://77.173.36.55/gateways/0xF400/rgb/1   post  plain   "status": true
  ->  Method: 2 URI: /gateways/0xF400/rgb/1   arg 'plain' = '"status": true'

*/
#include <WiFi.h>

void setup() {
  WiFiManager_Start();      //Setup all WIFI stuff (NOT INCLUDED!!, INCLUDE YOUR OWN OR INCLUDE IT YOURSELF)
  IFTTT_setup();            //start IFTTT, this will set the webser stuff up.
  
  Serial.begin(115200);
  Serial.println(WiFi.localIP()); //Just send it's IP on boot to let you know
}

void loop() {
  IFTTT_loop();             //handle the IFTTT web trafic
}
