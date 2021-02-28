#include "Arduino.h"

class Puller {
  public:
 
    Puller(int main_pinout, 
    int groud_pinout, 
    byte soft_start_coeff,
    float soft_start_length,
    int current_position,
    int max_length,
    double step_size,
    int weight,
    bool weight_on_backward) { 
      _groud_pinout = groud_pinout;
      _main_pinout = main_pinout;
      _soft_start_coeff = soft_start_coeff;
      _soft_start_length =  soft_start_length;
      _current_position = current_position;
      _max_length = max_length;
      _backward_direction = false;
      _step_size = step_size;
      _weight_compensation = ((double)weight)/32855;
      _weight_on_backward = weight_on_backward;
      Serial.println(weight/32855);
      Serial.println(_weight_compensation);
      pinMode(_main_pinout, OUTPUT);
      pinMode(_groud_pinout, OUTPUT);
      analogWriteFreq(100);
      _motion_in_progress = false;
    }
    
    void softStart() {
        Serial.print("SoftStart ");
        Serial.println(_soft_start_coeff);
        
        digitalWrite(_groud_pinout, LOW);
        for (int i=300; i<1024; i+=10) {
          analogWrite(_main_pinout, i);
          delay(_soft_start_coeff);
        }
        digitalWrite(_main_pinout, LOW);
        Serial.print("END SoftStart");
      }
    
    int move(int new_position){
      if (!_motion_in_progress){
        
        if (new_position < _current_position){
          setBackwardDirection();
        }else{
          setForwardDirection();
        }
        
        float delta = abs(new_position - _current_position)*(_max_length/100);
        
        Serial.print("DELTA in mm is ");
        Serial.println(delta);
        if (delta >= _soft_start_length+_step_size){       
          if (_weight_on_backward = _backward_direction){
            Serial.print("WEIGHT COMPENSATION ");
            delta += delta*_weight_compensation;
            Serial.println(delta);
          }
  
          _motion_in_progress = true;
          _rotate_time = max(ceil((delta-_soft_start_length)/_step_size),0.0);
          softStart();
          digitalWrite(_groud_pinout, LOW);
          analogWrite(_main_pinout, 1024);
          _previousMillis = millis();
          _current_position = new_position;
      }
     }
      return _current_position;
      
    }
    
    void motion_loop(){
      unsigned long currentMillis = millis();
      if (_motion_in_progress){
        if(currentMillis - _previousMillis >= _rotate_time){
          digitalWrite(_main_pinout, LOW);
          _motion_in_progress = false;
        }
      }
    }
      
  private:
    byte _soft_start_coeff;
    float _soft_start_length;
    int _max_length;
    float _current_position;
    int _main_pinout;
    int _groud_pinout;
    bool _backward_direction;
    bool _weight_on_backward;
    double _step_size;
    double _weight_compensation;
    bool _motion_in_progress;
    double _rotate_time;
    unsigned long _previousMillis;
    
        
    void setBackwardDirection(){
      if (!_backward_direction){
        _backward_direction = true;
        
        int temp = _main_pinout;
        _main_pinout = _groud_pinout;
        _groud_pinout = temp;
      }
    }
    
    void setForwardDirection(){
      if (_backward_direction){
        _backward_direction = false;
        
        int temp = _main_pinout;
        _main_pinout = _groud_pinout;
        _groud_pinout = temp;
      }
    }
};
