using simpleCallback = void (*)(void);
using parameterCallback = void (*)(int);

int _current_position;
int _target_position;
int _current_position_steps;
int _previous_position_steps;
unsigned long _previous_position_time;
int _target_position_steps;
int _main_pinout;
int _groud_pinout;
bool _motion_in_progress;
int _move_step;
int _move_speed = 0;
int _aceleration_period = 10;
unsigned long _last_time_run;
int _start_speed = 256;
int _max_position;
int _max_position_steps;

void deadLockCheck();

simpleCallback _stop_notify;
simpleCallback _opening_notify;
simpleCallback _closing_notify;
parameterCallback _current_position_notify;

void ICACHE_RAM_ATTR interruptHandler(void) {
    _current_position_steps += _move_step;
    _previous_position_time = millis();
}

void motorMoveForward(int speed){
  digitalWrite(_groud_pinout, LOW);
  analogWrite(_main_pinout, speed);
}

void motorMoveBackward(int speed){
  digitalWrite(_main_pinout, LOW);
  analogWrite(_groud_pinout, speed);
}
void motorStop(){
  digitalWrite(_groud_pinout, LOW);
  digitalWrite(_main_pinout, LOW);
}

void pullerInit(int main_pinout, 
  int groud_pinout,
  int interrupt_pin,
  simpleCallback stop_notify,
  simpleCallback opening_notify,
  simpleCallback closing_notify,
  parameterCallback current_position_notify ) { 
  
  _groud_pinout = groud_pinout;
  _main_pinout = main_pinout;
  pinMode(_main_pinout, OUTPUT);
  pinMode(_groud_pinout, OUTPUT);

  motorStop();
      
  pinMode(interrupt_pin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), interruptHandler, CHANGE); 
      
  analogWriteFreq(100);

  
  _current_position_steps = 0;
  _target_position_steps = _current_position_steps;
  _max_position = 100;
  _max_position_steps = 900;
  _current_position = 0;
  _target_position = 0;
  _move_step = 0;

  _stop_notify = stop_notify;
  _opening_notify = opening_notify;
  _closing_notify = closing_notify;
  _current_position_notify = current_position_notify;
  
  _current_position_notify(_current_position);
}

void pullerMove(int target_position){
  _target_position = target_position;
  if ((_move_step==1 && _current_position > target_position) ||
      (_move_step==-1 && _current_position < target_position) ){  
    _move_step = 0;
    _move_speed = 0;
    motorStop();
    _stop_notify();
  }
  if (_current_position == target_position){
    _current_position_steps =_target_position_steps;
    _current_position_notify(_current_position);
    _stop_notify();
    return;
  }
  if (_current_position < target_position){
    _target_position_steps = (int)((float)target_position/(float)_max_position*(float)_max_position_steps);
    //Serial.printf("Target position in steps %d \n", _target_position_steps);
    _opening_notify();
    return;
  }
  if (_current_position > target_position){
    _target_position_steps = (int)((float)target_position/(float)_max_position*(float)_max_position_steps);
    //Serial.printf("Target position in steps %d \n", _target_position_steps);
    _closing_notify();
    return;
  }
}

void pullerLoop(){
  if (_move_step != 0 && _current_position_steps == _target_position_steps){
    _move_step = 0;
    _move_speed = 0;
    motorStop();

    _current_position = (int)((float)_current_position_steps/(float)_max_position_steps*(float)_max_position);
    _current_position_notify(_target_position);
    _stop_notify();
    return;
  }
  if (_current_position_steps < _target_position_steps){
    _move_step = 1;

    if (_move_speed < 1024 && millis() - _last_time_run > _aceleration_period){
      _move_speed +=10;
      _move_speed = min(_move_speed, 1024);
      _last_time_run = millis();
    }
    motorMoveForward(_move_speed);
    
    _current_position = (int)((float)_current_position_steps/(float)_max_position_steps*(float)_max_position);
    
    deadLockCheck();
    return;
  }
  if (_current_position_steps > _target_position_steps){
    _move_step = -1;

    if (_move_speed < 1024 && millis() - _last_time_run > _aceleration_period){
      _move_speed +=10;
      _move_speed = min(_move_speed, 1024);
      _last_time_run = millis();

    }
    motorMoveBackward(_move_speed);
    
    _current_position = (int)((float)_current_position_steps/(float)_max_position_steps*(float)_max_position);
    
    deadLockCheck();
    return;
  }
}

void deadLockCheck(){
  if (_move_speed == 1024 && millis() - _previous_position_time >= 250 ){
      _move_step = 0;
      _move_speed = 0;
      motorStop();
      if (_target_position == 100 || _target_position == 0){
        // DEAD LOCK. END OF CHAIN
        _current_position_notify(_target_position);
        _current_position_steps = _target_position_steps;
        _current_position = _target_position;
      } else{
        // DEAD LOCK. MIDDLE BLOCK
        _stop_notify();
        _current_position_notify(_current_position);
        _target_position_steps = _current_position_steps;
        _target_position = _current_position;
      }
      _stop_notify();
    }
}
 
