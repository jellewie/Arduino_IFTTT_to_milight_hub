# IFTTT_to_milight_hub
 a web url (for IFTTT) to the esp8266_milight_hub

hub: [esp8266_milight_hub](https://github.com/sidoh/esp8266_milight_hub)

# What you need
[IFTTTMaker](https://github.com/witnessmenow/arduino-ifttt-maker)
[ArduinoJson](https://github.com/bblanchon/ArduinoJson)
[ESP32](https://dl.espressif.com/dl/package_esp32_index.json)

# How it works
Set-up your wifi/connection stuff (I use my own [Arduino-WiFiManager](https://github.com/jellewie/Arduino-WiFiManager) for it 

fillin the "KEYIFTTT" with the IFTTT key

fillin the "HubMiLight" with your esp8266_milight_hub ip (Sitenote: must be reachable for the ESP ofc.)

Call "IFTTT_setup()" to start, this will set the webser stuff up.

Call "IFTTT_loop()" to handle the IFTTT web trafic

If you want the outside (no not only local) to be able to acces this ESP. you NEED to open a port in your router, and port forward that to the <IESPIP>(with an optional gate of 80)!
