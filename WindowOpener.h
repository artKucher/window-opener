void saveConfigCallback();

void wifiManagerInit(){
  wifiManager.addParameter(&mqtt_server_param);
  wifiManager.addParameter(&mqtt_port_param);
  wifiManager.addParameter(&mqtt_login_param);
  wifiManager.addParameter(&mqtt_password_param);
  wifiManager.addParameter(&mqtt_input_topic_param);
  wifiManager.addParameter(&mqtt_output_topic_param);

  wifiManager.addParameter(&raindetector_wet_value_param);
  wifiManager.addParameter(&raindetector_dry_value_param);
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //sets timeout until configuration portal gets turned off
  wifiManager.setTimeout(120);
}

void initConnection(){
  if (!wifiManager.autoConnect(WIFI_SSID, WIFI_PASSWORD)) {
    //Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(5000);
  }
  //Serial.println("Connected to WiFi");
}

void resetSettings(){
  wifiManager.resetSettings();
  SPIFFS.remove("/pair.dat");
  SPIFFS.remove("/config.json");
  SPIFFS.format();
    
}


void checkWifiConnection(){
  if (WiFi.waitForConnectResult() != WL_CONNECTED){
    //Serial.println(F("WiFi is not connected"));
    ESP.reset();
    delay(5000);
  }
}

void getDataFromConfig(){
  //Serial.println("Getting Data from config");
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json")) {
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        if ( ! deserializeError ) {
          Serial.println("\nparsed json");
          serializeJson(json, Serial);
          strcpy(mqtt_server, json["mqtt_server"]);
          mqtt_port = json["mqtt_port"].as<int>();
          strcpy(mqtt_login, json["mqtt_login"]);
          strcpy(mqtt_password, json["mqtt_password"]);
          strcpy(mqtt_input_topic, json["mqtt_input_topic"]);
          strcpy(mqtt_output_topic, json["mqtt_output_topic"]);

          raindetector_wet_value = json["raindetector_wet_value"].as<int>();
          raindetector_dry_value = json["raindetector_dry_value"].as<int>();
        } else {
          //Serial.println("failed to load json config");
        }
        configFile.close();
      }
    } else{
      //Serial.println("File does not exists");
    }
  } else {
    //Serial.println("failed to mount FS");
  }
}


void saveConfigCallback(){
  Serial.println("saving config");
  strcpy(mqtt_server, mqtt_server_param.getValue());
  mqtt_port = String(mqtt_port_param.getValue()).toInt();
  strcpy(mqtt_login, mqtt_login_param.getValue());
  strcpy(mqtt_password, mqtt_password_param.getValue());
  strcpy(mqtt_input_topic, mqtt_input_topic_param.getValue());
  strcpy(mqtt_output_topic, mqtt_output_topic_param.getValue());

  raindetector_wet_value = String(raindetector_wet_value_param.getValue()).toInt();
  raindetector_dry_value = String(raindetector_dry_value_param.getValue()).toInt();
  
  DynamicJsonDocument json(1024);
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_login"] = mqtt_login;
  json["mqtt_password"] = mqtt_password;
  json["mqtt_input_topic"] = mqtt_input_topic;
  json["mqtt_output_topic"] = mqtt_output_topic;

  json["raindetector_wet_value"] = raindetector_wet_value;
  json["raindetector_dry_value"] = raindetector_dry_value;
  
  if (SPIFFS.begin()) {
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      //Serial.println("failed to open config file for writing");
    }
    serializeJson(json, configFile);
    configFile.close();
  }
  MqttInit();
  //Serial.println("MQTT INITED");
  if (!MqttReconnect() && strlen(mqtt_server) != 0) {
    //Serial.println("Failed to connect MQTT. Settings will be reseted.");
    
    resetSettings();
    ESP.reset();
  }
  
}
