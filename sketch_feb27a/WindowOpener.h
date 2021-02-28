#include <WiFiManager.h> 

char mqtt_server[40];
char mqtt_port[6];
WiFiManager wifiManager;
WiFiManagerParameter custom_mqtt_server{"server", "mqtt server", mqtt_server, 40};
WiFiManagerParameter custom_mqtt_port{"port", "mqtt port", mqtt_port, 6};


void saveConfigCallback(){
  Serial.println("saving config");
  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  //save the custom parameters to FS
  DynamicJsonDocument json(1024);
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  configFile.close();
}

void wifiManagerInit(){
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //sets timeout until configuration portal gets turned off
  wifiManager.setTimeout(120);
}

void initConnection(){
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(5000);
  }
  Serial.println("Connected to WiFi");
}

void debugModeOn(){
  //reset settings - for testing
  wifiManager.resetSettings();
  //clean FS, for testing
  SPIFFS.format();
}


void checkWifiConnection(){
  if (WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.println(F("WiFi is not connected"));
    ESP.reset();
    delay(5000);
  }
}

void getDataFromConfig(){
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);

        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if ( ! deserializeError ) {
          Serial.println("\nparsed json");
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
}
