#include "Arduino.h"

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

class WifiManagerCustom {
  public:
    //define your default values here, if there are different values in config.json, they are overwritten.
    char mqtt_server[40];
    char mqtt_port[6];
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);

      
    WifiManagerCustom(int main_pinout) { 
      _groud_pinout = groud_pinout;

    }
    
    void softStart() {
      }
      
    //callback notifying us of the need to save config
    void saveConfigCallback () {
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
      
  private:
    byte _soft_start_coeff;
    
    
    void setForwardDirection(){
      }
    }
};
