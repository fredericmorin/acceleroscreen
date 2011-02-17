
#ifndef SMOOTH_READER_H_
#define SMOOTH_READER_H_ 

#include "Reader.h"

class SmoothReader : public Reader {
private:
  Reader *_src;
  byte _count;
  byte pos;
  float *circBuffer;
public:
  SmoothReader(Reader *src, byte count) {
    _src = src;
    _count = count;
    pos = -1;
    circBuffer = (float*)malloc(sizeof(float) * count);
  }
  
  ~SmoothReader() {
    free(circBuffer);
  }
  
  float get() {
    float val = _src->get();
    if (pos == -1)
      for (pos = 0; pos < _count; pos++)
        circBuffer[pos] = val;
    circBuffer[pos++] = val;
    if (pos >= _count)
      pos = 0;
    return getAvg();
  }
  
private:
  float getAvg() {
    double value = 0;
    for (int i = 0; i < _count; i++)
      value += circBuffer[i];
    return value / _count;
  }
};


#endif // _ANALOG_READER_H_
