int _input_pin;
int _wet_value;
int _dry_value;

const int numReadings = 50;

int _readings[numReadings];      
int _read_index = 0;              
int _total = 0;      
int _average = 0;
bool _dryed = true;
unsigned long _last_time_measure = millis();
int _measure_period = 1000;



void rainDetectorInit(int input_pin, int wet_value, int dry_value) { 
  _input_pin = input_pin;
  _wet_value = wet_value;
  _dry_value = dry_value;
  
  for (int i = 0; i < numReadings; i++) {
    _readings[i] = analogRead(_input_pin);
    _total = _total + _readings[i];
    delay(2);
  }
}



void rainDetectorLoop(){
  if (millis() - _last_time_measure <= _measure_period){
    return;
  }
  _total = _total - _readings[_read_index];
  _readings[_read_index] = analogRead(_input_pin);
  _total = _total + _readings[_read_index];
  _read_index++;

  if (_read_index >= numReadings) {
    _read_index = 0;
  }

  _average = _total / numReadings;
  if (_average <= _dry_value){
    _dryed = true;
  }
  //Serial.println(_average);
  
  if (_average >= _wet_value && _dryed){
    pullerMove(0);
    _dryed = false;
  }
  _last_time_measure = millis();
}
