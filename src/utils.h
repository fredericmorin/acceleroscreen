/*
 * util.h
 *
 *  Created on: 2011-02-20
 *      Author: fred
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <avr/pgmspace.h>

inline void spi_transfer(uint8_t data) {
	SPDR = data; // Start the transmission
	while (!(SPSR & (1 << SPIF)))
		; // Wait the end of the transmission
}

inline long maplimit(long x, long in_min, long in_max, long out_min, long out_max) {
	long res = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	res = ((out_min) > (res) ? (out_min) : (res));
	res = ((out_max) < (res) ? (out_max) : (res));
	return res;
}

void quickDigitalWrite(uint8_t pin, uint8_t val);

#endif /* UTIL_H_ */
