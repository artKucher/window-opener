#include <ArduinoJson.h>

int mqtt_last_value = 0;

void onMessage(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<128> jsonInBuffer;
  DeserializationError error = deserializeJson(jsonInBuffer, payload);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  String data = jsonInBuffer["data"];
  int value = data.toInt();
  Serial.println(data);
  Serial.printf("Value %d \n",value);
  if (value<=100 && value>=0){
    pullerMove(value);
  }
}

boolean MqttReconnect() {
  Serial.println("RECONNECT");
  if (mqttClient.connect(MQTT_CLIENTID, mqtt_login, mqtt_password)) {
    mqttClient.subscribe(mqtt_input_topic);
    Serial.println("Connected to Beebotte MQTT");
  }
  return mqttClient.connected();
}

void MqttInit(){
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(onMessage);
  MqttReconnect();
}

void MqttLoop(){
  Serial.printf("MQTT status %d \n", mqttClient.state());
  if (!mqttClient.loop()) {
    MqttReconnect();
    
  }
}

void mqtt_set_current_position(int current_pos){
  if (strlen(mqtt_output_topic) != 0 and mqtt_last_value != current_pos ){
    DynamicJsonDocument json(128);
    json["ispublic"] = true;
    json["ts"] = millis();
    
    JsonObject data = json.createNestedObject("data");
    data["device_name"] = MQTT_CLIENTID;
    data["type"] = "current position";
    data["data"] = current_pos;
  
    char message[128];
    serializeJson(json, message);
    mqttClient.publish(mqtt_output_topic, message);
    mqtt_last_value = current_pos;
  }
}
void mqtt_stop_notify(){
  if (strlen(mqtt_output_topic) != 0 ){
    DynamicJsonDocument json(128);
    json["ispublic"] = true;
    json["ts"] = millis();
    
    JsonObject data = json.createNestedObject("data");
    data["device_name"] = MQTT_CLIENTID;
    data["type"] = "motion status";
    data["data"] = "stopped";
  
    char message[128];
    serializeJson(json, message);
    mqttClient.publish(mqtt_output_topic, message);
  }

}
void mqtt_opening_notify(){
  if (strlen(mqtt_output_topic) != 0 ){
    DynamicJsonDocument json(128);
    json["ispublic"] = true;
    json["ts"] = millis();
    
    JsonObject data = json.createNestedObject("data");
    data["device_name"] = MQTT_CLIENTID;
    data["type"] = "motion status";
    data["data"] = "opening";
  
    char message[128];
    serializeJson(json, message);
    mqttClient.publish(mqtt_output_topic, message);
  }
}
void mqtt_closing_notify(){
  if (strlen(mqtt_output_topic) != 0 ){
    DynamicJsonDocument json(128);
    json["ispublic"] = true;
    json["ts"] = millis();
    
    JsonObject data = json.createNestedObject("data");
    data["device_name"] = MQTT_CLIENTID;
    data["type"] = "motion status";
    data["data"] = "closing";
  
    char message[128];
    serializeJson(json, message);
    mqttClient.publish(mqtt_output_topic, message);
  }
}
