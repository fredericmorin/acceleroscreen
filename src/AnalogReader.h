
#ifndef ANALOG_READER_H_
#define ANALOG_READER_H_ 

#include "Reader.h"

class AnalogReader : public Reader {
private:
  uint8_t _pin;
  long _from;
  long _to;
  
public:
  AnalogReader(uint8_t pin, float from, float to) {
    _pin = pin;
    _from = (long) (from * 1024);
    _to = (long) (to * 1024);
  }
  
  long getRaw() const {
    return analogRead(_pin);
  }
  
  float get() {
    return map(analogRead(_pin), 0, 1023, _from, _to) / 1000.0;
  }
  
  int getInt() const {
    return map(analogRead(_pin), 0, 1023, _from, _to);
  }
  
};


#endif // _ANALOG_READER_H_
