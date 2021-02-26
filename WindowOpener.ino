#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include "coredecls.h"
extern "C"{
#include "homeintegration.h"
}
#include "homekitintegrationcpp.h"
#include <hapfilestorage/hapfilestorage.hpp>
#include <hapweb/hap_webserver.hpp>
#include "file_index_html.h"
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager
#include "Puller.cpp"
#include <Thread.h>

Puller puller(4,12,20,9.83,0,300, 7.5/1000, 515, true);
Thread pullerThread = Thread();

bool isWebserver_started=false;
const int identity_led = 4;
const int led_gpio = 2;

const char* HOSTNAME="WindowCover";
homekit_service_t* hapservice={0};
uint8_t led_level_srored=0;

void startwifimanager() {
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect(HOSTNAME, NULL)) {
      ESP.restart();
      delay(1000);
   }
}

void setup() {
  homekit_storage_reset();

  disable_extra4k_at_link_time();
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  if (!SPIFFS.begin()) {
    Serial.print("SPIFFS Mount failed");
    }
 
   pinMode(led_gpio,OUTPUT);
   
   startwifimanager();
   ///setup identity gpio
   hap_set_identity_gpio(identity_led);    //identity_led=2 will blink on identity

    /// now will setup homekit device

    //this is for custom storaage usage
    // In given example we are using \pair.dat   file in our spiffs system
    SPIFFS.remove("/pair.dat");
    init_hap_storage("/pair.dat");


    /// We will use for this example only one accessory (possible to use a several on the same esp)
    //Our accessory type is light bulb , apple interface will proper show that
    hap_setbase_accessorytype(homekit_accessory_category_window_covering);
    
    // Setup ID in format "XXXX" (where X is digit or latin capital letter)
    // Used for pairing using QR code
    hap_set_device_setupId((char*)"YK72");
    /// init base properties
    hap_initbase_accessory_service(HOSTNAME,"Yurik72","0","EspHapLed","1.0");

   //we will add only one light bulb service and keep pointer for nest using
    hapservice= hap_add_relaydim_service("Led",led_callback,(void*)&led_gpio);

   //and finally init HAP

    hap_init_homekit_server();
  
   
    set_indexhml(FPSTR(INDEX_HTML));
    hap_webserver_begin();
      
    server.on("/get", handleGetVal);
    server.on("/set", handleSetVal);

    isWebserver_started=true;
}

void loop() {
  hap_homekit_loop();
  
  if(isWebserver_started){
     hap_webserver_loop();
  }
  puller.motion_loop();

}

bool get_led(){
  if(hapservice){
      homekit_characteristic_t * ch= homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_ON);
      if(ch){
        return ch->value.bool_value;
      }
    }
    return false;
}

uint8_t get_led_level(){
  if(hapservice){
      homekit_characteristic_t * ch= homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_BRIGHTNESS);
      if(ch){
        return ch->value.int_value;
      }
    }
    return 0;
}
void handleGetVal(){
  if(server.arg("var") == "ch1")
    server.send(200, FPSTR(TEXT_PLAIN),String(get_led()));
  else if(server.arg("var") == "level")
     server.send(200, FPSTR(TEXT_PLAIN),String(get_led_level()));
  else
    server.send(505, FPSTR(TEXT_PLAIN),"Bad args");  
}
void handleSetVal(){
  if (server.args() !=2){
    server.send(505,"text/plain", "Bad args");
    return;
  }
   bool isOk=false;
  
  if(server.arg("var") == "ch1"){
    if(hapservice){
      homekit_characteristic_t * ch= homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_ON);
      if(ch){
        set_led(server.arg("val")=="true");
        isOk=true;
      }
    }
  }
   if(server.arg("var") == "level"){
    if(hapservice){

      homekit_characteristic_t * ch= homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_BRIGHTNESS);
      if(ch){
        set_led_level(String(server.arg("val")).toInt());
         isOk=true;
      }
    }
  }
  if( isOk)
    server.send(200, FPSTR(TEXT_PLAIN), "OK");
  else
    server.send(505, FPSTR(TEXT_PLAIN), "WRONG");
}
//can be used for any logic, it will automatically inform Apple about state changes
void set_led(bool val){
  Serial.println("set_led"); 
  
  //we need notify apple about changes
  if(hapservice){
      Serial.println("notify hap state"); 
      homekit_characteristic_t * ch= homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_ON);
      HAP_NOTIFY_CHANGES(bool, ch, val, 0)
  }
  if(!val){
    set_led_level(0);
  }
  else{
    set_led_level(led_level_srored);
  }
  
}
void set_led_level(uint8_t val){
  Serial.println("set_led_level");
  Serial.println(val);
  //analogWrite(led_gpio, DIMCALC_VAL(MAP_100_2_255(val), false));
  val = puller.move(val);

  if(hapservice){
    Serial.println("notify led level:"+String(val)); 
    //getting on/off characteristic
    homekit_characteristic_t * ch= homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_BRIGHTNESS);
    HAP_NOTIFY_CHANGES(int, ch, val, 0)
    if(val>0)
      led_level_srored=val;
  } 
  
}

void led_callback(homekit_characteristic_t *ch, homekit_value_t value, void *context) {
    Serial.println("led_callback");
  
    if(strcmp(ch->type,HOMEKIT_CHARACTERISTIC_ON)==0)
      set_led(ch->value.bool_value);
    else if(strcmp(ch->type,HOMEKIT_CHARACTERISTIC_BRIGHTNESS)==0)
      set_led_level(ch->value.int_value);
    else
      Serial.println(" unknown charactheristic"); 
  
}
