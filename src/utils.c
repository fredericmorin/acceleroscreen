/*
 * util.h
 *
 *  Created on: 2011-02-20
 *      Author: fred
 */

#include "utils.h"

#include "WProgram.h"
#include "pins_arduino.h"

void quickDigitalWrite(uint8_t pin, uint8_t val) {
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
