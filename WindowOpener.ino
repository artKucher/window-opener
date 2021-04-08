//#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
//#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "vars.h"
#include "Puller_new.h"
#include "Mqtt.h"
#include "WindowOpener.h"
#include "Hap.h"


void setup() {
  Serial.begin(115200);
  
  //resetSettings();
  
  wifiManagerInit();
  initConnection();
  ArduinoOTA.begin();
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  getDataFromConfig();
  if (strlen(mqtt_server) != 0){
    Serial.println("INIT MQTT ONLY");
    MqttInit();
    ComanderLoop = MqttLoop;
    pullerInit(4,5,2, mqtt_stop_notify, mqtt_opening_notify, mqtt_closing_notify, mqtt_set_current_position);
  }else{
    Serial.println("INIT HAP ONLY");
    initHap();
    ComanderLoop = hap_homekit_loop;
    pullerInit(4,5,2, hap_stop_notify, hap_opening_notify, hap_closing_notify, hap_set_current_position);
  }
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  checkWifiConnection();
  ComanderLoop();
  pullerLoop();
}
