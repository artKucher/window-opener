#include <Arduino.h>
extern "C"{
#include "homeintegration.h"
}
#include "homekitintegrationcpp.h"
#include <hapfilestorage/hapfilestorage.hpp>

#include <ESP8266mDNS.h>
#include "coredecls.h"
#include <ESP8266WiFi.h>


#define INFO_(message, ...) printf_P(PSTR(">>> Window covering: " message "\n"), ##__VA_ARGS__)
#define ERROR_(message, ...) printf_P(PSTR("!!! Window covering: " message "\n"), ##__VA_ARGS__)


const char* HOSTNAME="WindowCover";
const int identity_led=2;

homekit_service_t* service_windowcovering=NULL;
homekit_characteristic_t * ch_current_pos= NULL;
homekit_characteristic_t * ch_target_pos=NULL;
homekit_characteristic_t * ch_position_state=NULL;

homekit_characteristic_t * ch_hold=NULL;


void hap_callback_process(homekit_characteristic_t *ch, homekit_value_t value, void *context) {

    
    //Serial.printf("HAP CALLBACK. Value: %d - %d, type: %s, description %s \n", value.int_value, ch->value.int_value, ch->type, ch->description);
    
    if(!service_windowcovering || !ch_target_pos || !ch_position_state){
       ERROR_("service/charachteristic are not defined \n");
      return;
   
    }

    const char* type_tp = "7C";
    if (strcmp(ch->type, type_tp) == 0){
      ch_target_pos->value.int_value = value.int_value;
      pullerMove(value.int_value);
    }
}

void hap_set_current_position(int current_pos){
  //Serial.printf("SETTING CURRENT POSITION %d \n", current_pos);
  homekit_characteristic_t * ch= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_CURRENT_POSITION);
  HAP_NOTIFY_CHANGES(int, ch, current_pos, 1)
}
void hap_stop_notify(){
  //Serial.println("STOPPED NOTIFY HAP");
  homekit_characteristic_t * ch= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_POSITION_STATE);
  HAP_NOTIFY_CHANGES(int, ch, WINDOWCOVERING_POSITION_STATE_STOPPED, 0)
}
void hap_opening_notify(){
  //Serial.println("OPENING NOTIFY HAP");
  homekit_characteristic_t * ch= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_POSITION_STATE);
  HAP_NOTIFY_CHANGES(int, ch, WINDOWCOVERING_POSITION_STATE_OPENING, 0)
}
void hap_closing_notify(){
  //Serial.println("CLOSING NOTIFY HAP");
  homekit_characteristic_t * ch= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_POSITION_STATE);
  HAP_NOTIFY_CHANGES(int, ch, WINDOWCOVERING_POSITION_STATE_CLOSING, 0)
}



void initHap(){
    Serial.println("InitiateHAP");
    disable_extra4k_at_link_time();
    init_hap_storage("/pair.dat");
    hap_setbase_accessorytype(homekit_accessory_category_window_covering);
    hap_set_device_setupId((char*)"YK72");
    hap_initbase_accessory_service(HOSTNAME,"Yurik72","0","EspHapLed","1.0");
    service_windowcovering = hap_add_windowcovering_service("Window",hap_callback_process,0); 
    hap_init_homekit_server();
    ch_current_pos= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_CURRENT_POSITION);
    ch_target_pos= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_TARGET_POSITION);
    ch_position_state= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_POSITION_STATE);
    //Set  default values
    INIT_CHARACHTERISTIC_VAL(int,ch_current_pos,0);
    INIT_CHARACHTERISTIC_VAL(int,ch_target_pos,0);
}
