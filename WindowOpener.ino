#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPUpdateServer.h>

#include "vars.h"
#include "Puller.h"
#include "Mqtt.h"
#include "WindowOpener.h"
#include "Hap.h"
#include "RainDetector.h"

const char* update_username = "admin";
const char* update_password = "admin";
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  Serial.begin(115200);
  
  wifiManagerInit();
  initConnection();
  
  Serial.println("Ready");

  getDataFromConfig();
  if (strlen(mqtt_server) != 0){
    MqttInit();
    ComanderLoop = MqttLoop;
    pullerInit(4,5,12, mqtt_stop_notify, mqtt_opening_notify, mqtt_closing_notify, mqtt_set_current_position);
  }else{
    initHap();
    ComanderLoop = hap_homekit_loop;
    pullerInit(4,5,12, hap_stop_notify, hap_opening_notify, hap_closing_notify, hap_set_current_position);
  }
  rainDetectorInit(A0, raindetector_wet_value, raindetector_dry_value);
  httpUpdater.setup(&httpServer, "/firmware", update_username, update_password);
  httpServer.begin();
}

void loop() {
  checkWifiConnection();
  ComanderLoop();
  pullerLoop();
  httpServer.handleClient();
  rainDetectorLoop();
}
