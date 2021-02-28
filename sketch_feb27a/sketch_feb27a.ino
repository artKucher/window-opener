#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "WindowOpener.h"
#include "Hap.h"

void setup() {
  Serial.begin(115200);
  
  debugModeOn();
  
  getDataFromConfig();
  wifiManagerInit();
  initConnection();
  ArduinoOTA.begin();
  
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initHap();
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  checkWifiConnection();
  hap_homekit_loop();
  update_window_state();
}
