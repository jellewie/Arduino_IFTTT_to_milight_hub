/* Written by JelleWho https://github.com/jellewie

   TODO
   Polish it up, test it even more
   Get data back from ESP?
   Make it so 'TurnOn' isn't needed with other commands. -> if(colorset and TurnOn == ""){TurnOn=true}
   check length StaticJsonBuffer

   DO NOT USE:
   Prefix 'IFTTT' for anything
   Global variables of 'i' 'j' 'k' 'client'
*/

#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <IFTTTMaker.h>

//#define IFTTT_SerialEnabled

#define IFTTT_KEYIFTTT "REPLACE THIS WITH YOU IFTTT KEY"
#define IFTTT_HubMiLight "192.168.1.128"  //The miligh hub IP

#define IFTTT_MaxTransitionIntervalPeriod 60*1000   //Length of time between updates in a transition, in ms
#define IFTTT_MaxTransitionDuration 10*60           //Duration of transition, in seconds!
#define IFTTT_DefaultTransitionStartBrightness 0
#define IFTTT_DefaultTransitionEndBrightness 255
#define IFTTT_DefaultTransitionperiod 1000

WebServer IFTTT_server(80);
WiFiClientSecure client;
IFTTTMaker ifttt(IFTTT_KEYIFTTT, client);

void IFTTT_setup() {
  IFTTT_server.on("/set",  IFTTT_handle_Set);
  IFTTT_server.onNotFound( IFTTT_HandleNotFound);
  IFTTT_server.begin();
}
void IFTTT_loop() {
  IFTTT_server.handleClient();
}
void IFTTT_handle_Set() {
  //URL argument names like "<IP>/set?<PreFixDeviceID>=<value>[&<PreFixSomething2>=<value>]" where [] is optional stuff.
  //Example                 "192.168.1.128/set?light=0xF241&on=true"
  String IFTTT_ReturnMSG;
  byte IFTTT_ReturnStatus = 422;  //set this error as default
  String IFTTT_device_id, IFTTT_Red, IFTTT_Green, IFTTT_Blue, IFTTT_TurnOn, IFTTT_brightness, IFTTT_color_temp, IFTTT_start_value, IFTTT_end_value, IFTTT_duration, IFTTT_period, IFTTT_remote_type, IFTTT_group_id;  //emthy=dont change
  for (int i = 0; i < IFTTT_server.args(); i++) {
    String IFTTT_ArguName = IFTTT_server.argName(i);
    IFTTT_ArguName.toLowerCase();
    String IFTTT_ArgValue = IFTTT_server.arg(i);
    IFTTT_ArgValue.toLowerCase();

    if (IFTTT_ArguName == "light")              //PreFixDeviceID
      IFTTT_device_id = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "on")            //PreFixSetOnState
      IFTTT_TurnOn = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "r")             //PreFixSetColorValueRed
      IFTTT_Red = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "g")             //PreFixSetColorValueGreen
      IFTTT_Green = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "b")             //PreFixSetColorValueBlue
      IFTTT_Blue = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "b")             //PreFixSetBrightness
      IFTTT_brightness = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "l")             //PreFixSetcolorTemp
      IFTTT_color_temp = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "start_value")   //PreFixSetstart_value
      IFTTT_start_value = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "end_value")     //PreFixSetend_value
      IFTTT_end_value = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "duration")      //PreFixSetduration
      IFTTT_duration = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "period")        //PreFixSetperiod
      IFTTT_period = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "remote_type")   //PreFixSetremote_type
      IFTTT_remote_type = IFTTT_ArgValue;
    else if (IFTTT_ArguName == "group_id")      //PreFixSetgroup_id
      IFTTT_group_id = IFTTT_ArgValue;
    else
      IFTTT_ReturnMSG = ("Unknown arg '" + IFTTT_ArguName + "' with value '" + IFTTT_ArgValue + "'");
  }
  if (IFTTT_device_id != "") {
    //<HubMiLight>/gateways/<deviceIDHex>/<Mode of control>/<Group>
    StaticJsonBuffer<255> IFTTT_jsonBuffer;
    JsonObject& IFTTT_root = IFTTT_jsonBuffer.createObject();

    if (IFTTT_group_id == "")
      IFTTT_group_id = 1;
    else if (!IFTTT_NumberAndBelow(IFTTT_group_id, 5)) {
      IFTTT_group_id = "1";
      IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."
    }

    if (IFTTT_remote_type == "")
      IFTTT_remote_type = "rgb_cct";

    if (IFTTT_TurnOn == "true" or IFTTT_TurnOn == "false")
      IFTTT_root["status"] = IFTTT_TurnOn;
    else if (IFTTT_TurnOn != "") {
      IFTTT_TurnOn = "";
      IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."
    }

    if (IFTTT_NumberAndBelow(IFTTT_brightness, 256)) {
      if (IFTTT_TurnOn = "")
        IFTTT_root["status"] = "true";
      IFTTT_root["brightness"] = IFTTT_brightness;
    } else if (IFTTT_brightness != "")
      IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."

    if (IFTTT_NumberAndBelow(IFTTT_color_temp, 100000)) {
      if (IFTTT_TurnOn = "")
        IFTTT_root["status"] = "true";
      IFTTT_root["color_temp"] = IFTTT_color_temp;
    } else if (IFTTT_color_temp != "")
      IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."

    if (IFTTT_Red != "" or IFTTT_Green != "" or IFTTT_Blue != "") {
      if (IFTTT_NumberAndBelow(IFTTT_Red, 256) and IFTTT_NumberAndBelow(IFTTT_Green, 256) and IFTTT_NumberAndBelow(IFTTT_Blue, 256))
        IFTTT_root["color"] = "{\"r\": " + IFTTT_Red + ", \"g\": " + IFTTT_Green + ", \"b\": " + IFTTT_Blue + "}";
      else
        IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."
    }

    String IFTTT_path = "/gateways/" + IFTTT_device_id + "/" + IFTTT_remote_type + "/" + IFTTT_group_id;
    //====Transitions====
    if (IFTTT_NumberAndBelow(IFTTT_duration, IFTTT_MaxTransitionDuration)) {
      IFTTT_root["field"] = "brightness";

      if (IFTTT_NumberAndBelow(IFTTT_start_value, 256))
        IFTTT_root["start_value"] = IFTTT_start_value;            //Start value of brightness
      else {
        if (IFTTT_start_value != "")
          IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."
        IFTTT_root["start_value"] = IFTTT_DefaultTransitionStartBrightness;
      }

      if (IFTTT_NumberAndBelow(IFTTT_end_value, 256))
        IFTTT_root["end_value"] = IFTTT_end_value;            //Start value of brightness
      else {
        if (IFTTT_end_value != "")
          IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."
        IFTTT_root["end_value"] = IFTTT_DefaultTransitionEndBrightness;
      }

      if (IFTTT_NumberAndBelow(IFTTT_period, IFTTT_MaxTransitionIntervalPeriod))
        IFTTT_root["period"] = IFTTT_period;                      //time between updates
      else {
        if (IFTTT_period != "")
          IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."
        IFTTT_root["period"] = IFTTT_DefaultTransitionperiod;
      }
      IFTTT_root["duration"] = IFTTT_duration;

      if (IFTTT_device_id.substring(1, 3) = "0x")   //If it's HEX, convert to INT
        IFTTT_device_id = IFTTT_StringToHex(IFTTT_device_id.substring(2));
      IFTTT_root["device_id"] = IFTTT_device_id;
      IFTTT_root["group_id"] = IFTTT_group_id;
      IFTTT_root["remote_type"] = IFTTT_remote_type;

      IFTTT_path = "/transitions";
    }
    else if (IFTTT_duration != "")
      IFTTT_ReturnMSG = IFTTT_ReturnMSG + "\n\r" + "422";     //"The request failed because of invalid parameters."

    WiFiClient client;
    if (!client.connect(IFTTT_HubMiLight, 80)) {
      IFTTT_ReturnStatus = 500;   //The request failed because the downstream service could not process the request.
      return;
    }

    client.println("POST " + IFTTT_path + " HTTP/1.1");
    client.println("Host: " + String(IFTTT_HubMiLight));
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(IFTTT_root.measureLength()));
    client.println();
    IFTTT_root.printTo(client);
#ifdef SerialEnabled
    Serial.println("Connect, data send to hub:");
    Serial.println("path=" + String(IFTTT_HubMiLight) + IFTTT_path);
    root.printTo(Serial);
    Serial.println();
#endif //SerialEnabled

    if (IFTTT_ReturnMSG == ""){  //If no error MSG
      IFTTT_ReturnStatus = 200; //Return 'OK, succes' flag
      IFTTT_ReturnMSG = "Succesfull send";
    }
    IFTTT_server.send(IFTTT_ReturnStatus, "text/plain", "'" + String(IFTTT_ReturnMSG) + "'");
    return;
  }
  IFTTT_server.send(400);   //'The request was invalid.'
}

bool IFTTT_NumberAndBelow(String IFTTT_Input, int IFTTT_MAX) {
  if (IFTTT_Input != "")                //If its not emthy
    if (IFTTT_IsNumber(IFTTT_Input))          //If a number was given
      if (IFTTT_Input.toInt() < IFTTT_MAX)    //If it's below MAX
        return true;              //Return true
  return false;
}
bool IFTTT_IsNumber(String IFTTT_Input) {
  for (byte i = 0; i < IFTTT_Input.length(); i++) {
    if (!isDigit(IFTTT_Input.charAt(i)))
      return false;
  }
  return true;
}
void IFTTT_HandleNotFound() {
#ifdef IFTTT_SerialEnabled
  Serial.println("Method: " + String(IFTTT_server.method()) + " URI: " + IFTTT_server.uri());
  for (int i = 0; i < IFTTT_server.args(); i++)
    Serial.println("arg '" + String(IFTTT_server.argName(i)) + "' = '" + String(IFTTT_server.arg(i)) + "'");
#endif //SerialEnabled
}
void IFTTT_SendIFTTT() {
  if (ifttt.triggerEvent("Test", "waarde 1", "waarde 2", "waarde 3")) {
#ifdef IFTTT_SerialEnabled
    Serial.println("IFTTT Successfully sent");
#endif //SerialEnabled
  } else {
#ifdef IFTTT_SerialEnabled
    Serial.println("IFTTTFailed");
#endif //SerialEnabled
  }
}
int IFTTT_StringToHex(String Input) {
  int j = 0;
  for (byte i = 0; i < Input.length(); i++) {
    String k = Input.substring(i, i + 1);
    k.toLowerCase();
    if (k == "a") {
      k = 10;
    } else if (k == "b") {
      k = 11;
    } else if (k == "c") {
      k = 12;
    } else if (k == "d") {
      k = 13;
    } else if (k == "e") {
      k = 14;
    } else if (k == "f") {
      k = 15;
    } else if (k.toInt() < 10) {}
    else
      break;
    j *= 16;
    j += k.toInt();
  }
  return j;
}
