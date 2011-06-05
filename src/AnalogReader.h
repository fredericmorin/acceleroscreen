#ifndef ANALOG_READER_H_
#define ANALOG_READER_H_ 

#include "WProgram.h"

class AnalogReader {
private:
	uint8_t _pin;
	int16_t _from;
	int16_t _to;

public:
	AnalogReader(uint8_t pin, float from, float to) {
		_pin = pin;
		_from = (int16_t) (from * 1024);
		_to = (int16_t) (to * 1024);
		pinMode(pin, INPUT);
	}

	inline int16_t getRaw() const {
		return analogRead(_pin);
	}

	inline float get() const {
		return map(analogRead(_pin), 0, 1023, _from, _to) / 1000.0;
	}

	inline int32_t getLong() const {
		return map(analogRead(_pin), 0, 1023, _from, _to);
	}

};

#endif // _ANALOG_READER_H_
