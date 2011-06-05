/*
 * util.h
 *
 *  Created on: 2011-02-20
 *      Author: fred
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <avr/pgmspace.h>

#include "WProgram.h"
#include "pins_arduino.h"

inline void spi_transfer(uint8_t data) {
	SPDR = data; // Start the transmission
	while (!(SPSR & (1 << SPIF)))
		; // Wait the end of the transmission
}

template<typename T>
inline T maplimit(T x, T in_min, T in_max, T out_min, T out_max) {
	T res = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	res = ((out_min) > (res) ? (out_min) : (res));
	res = ((out_max) < (res) ? (out_max) : (res));
	return res;
}

inline void quickDigitalWrite(uint8_t pin, uint8_t val) {
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out = portOutputRegister(port);
	uint8_t oldSREG = SREG;
	cli();
	if (val == LOW) {
		*out &= ~digitalPinToBitMask(pin);
	} else {
		*out |= digitalPinToBitMask(pin);
	}
	SREG = oldSREG;
}

#endif /* UTIL_H_ */
