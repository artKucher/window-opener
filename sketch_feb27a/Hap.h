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


void handle_motor(){
  if(!ch_target_pos || !ch_position_state || !ch_current_pos ){
    ERROR_("Positions charachteristic are not defined\n");
    return;
  }
  uint8_t current_position = ch_current_pos->value.int_value;
  int8_t target_position = ch_target_pos->value.int_value ;
  INFO_("Handle Motor State:%d, Current:%d, Target:%d\n",ch_position_state->value.int_value,current_position,target_position);  
  switch(ch_position_state->value.int_value){
    case WINDOWCOVERING_POSITION_STATE_OPENING:
      INFO_("Handle motor OPEN \n");
      break;
    case WINDOWCOVERING_POSITION_STATE_CLOSING:
      INFO_("Handle motor CLOSE \n");
      break;
    case WINDOWCOVERING_POSITION_STATE_STOPPED:
     INFO_("Handle motor STOP \n");
      break;
    default:
     ERROR_("Unknown value of target position charachteristic\n");
  }
}

void update_window_state() {
        uint8_t position = ch_current_pos->value.int_value;
        int8_t direction = 0;
        if(ch_position_state->value.int_value == WINDOWCOVERING_POSITION_STATE_OPENING)
          direction+=1;
        if(ch_position_state->value.int_value == WINDOWCOVERING_POSITION_STATE_CLOSING)
          direction+=-1;

        int16_t newPosition = position + direction;
        

        INFO_("position %u, target %u\n", newPosition, ch_target_pos->value.int_value);
        HAP_NOTIFY_CHANGES(int, ch_current_pos, newPosition, 0)


        if (newPosition == ch_target_pos->value.int_value) {
            INFO_("reached destination %u\n", newPosition);
            HAP_NOTIFY_CHANGES(int, ch_position_state, WINDOWCOVERING_POSITION_STATE_STOPPED,0)
            suspend_update_task();
        }
        handle_motor();
}


void update_window_state_arg(void* arg){
  update_window_state();
}

void hap_callback_process(homekit_characteristic_t *ch, homekit_value_t value, void *context) {
    INFO_("hap_callback \n");
    if(!service_windowcovering || !ch_target_pos || !ch_position_state){
       ERROR_("service/charachteristic are not defined \n");
      return;
   
    }
    if (ch_target_pos->value.int_value == ch_current_pos->value.int_value) {
        INFO_("Current position equal to target. Stopping.\n");
        HAP_NOTIFY_CHANGES(int, ch_position_state, WINDOWCOVERING_POSITION_STATE_STOPPED,0)
    } else {
        int newState= ch_target_pos->value.int_value > ch_current_pos->value.int_value
            ? WINDOWCOVERING_POSITION_STATE_OPENING
            : WINDOWCOVERING_POSITION_STATE_CLOSING;
        HAP_NOTIFY_CHANGES(int, ch_position_state, newState,0)
    }
}

void initHap(){
    Serial.println("InitiateHAP");
    
    SPIFFS.remove("/pair.dat");
    init_hap_storage("/pair.dat");
    hap_setbase_accessorytype(homekit_accessory_category_window_covering);
    hap_set_device_setupId((char*)"YK72");
    hap_initbase_accessory_service(HOSTNAME,"Yurik72","0","EspHapLed","1.0");
    service_windowcovering = hap_add_windowcovering_service("Window",hap_callback_process,0);
    create_update_task();   
    hap_init_homekit_server();
    ch_current_pos= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_CURRENT_POSITION);
    ch_target_pos= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_TARGET_POSITION);
    ch_position_state= homekit_service_characteristic_by_type(service_windowcovering, HOMEKIT_CHARACTERISTIC_POSITION_STATE);
    //Set  default values
    INIT_CHARACHTERISTIC_VAL(int,ch_current_pos,50);
    INIT_CHARACHTERISTIC_VAL(int,ch_target_pos,50);
}
