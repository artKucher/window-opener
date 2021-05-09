void resetSettings();

#include "src/WiFiManager/WiFiManager.h"
#include "src/PubSubClient/PubSubClient.h"

char mqtt_server[40];
int mqtt_port;
char mqtt_login[64];
char mqtt_password[64];
char mqtt_input_topic[256];
char mqtt_output_topic[256];

int raindetector_wet_value = 600;
int raindetector_dry_value = 100;


WiFiManager wifiManager;
WiFiManagerParameter mqtt_server_param{"server", "mqtt server", mqtt_server, 40};
WiFiManagerParameter mqtt_port_param{"port", "mqtt port", "", 6};
WiFiManagerParameter mqtt_login_param{"login", "mqtt login", mqtt_login, 64};
WiFiManagerParameter mqtt_password_param{"password", "mqtt password", mqtt_password, 64};
WiFiManagerParameter mqtt_input_topic_param{"input topic", "mqtt input topic", mqtt_input_topic, 256};
WiFiManagerParameter mqtt_output_topic_param{"output topic", "mqtt output topic", mqtt_output_topic, 256};

WiFiManagerParameter raindetector_wet_value_param{"raindetector_wet_value", "Датчик дождя. Значение срабатывания", "600", 4};
WiFiManagerParameter raindetector_dry_value_param{"raindetector_dry_value", "Датчик дождя. Значение сухого", "300", 4};


WiFiClient espClient;
PubSubClient mqttClient(espClient);

//HAP SETTINGS
const char* HAP_HOSTNAME="WindowOpener_000000";
const char* device_setupId="WC72";
const char* HAP_MANUFACTURER="ymdom.ru";
const char* HAP_MODEL="Window Opener";
const char* HAP_SERIALNUMBER="000000";
const char* HAP_FIRMWAREVERSION="1.0";
//MQTT SETTINGS
const char* MQTT_CLIENTID="WindowOpener_000000";
//WI-FI settings
const char* WIFI_SSID="ymdom.ru_WindowOpener_000000";
const char* WIFI_PASSWORD="password";

void (*ComanderInit)();
void (*ComanderLoop)();
