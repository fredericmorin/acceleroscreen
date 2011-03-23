/*
 * screen.h
 *
 *  Created on: 2011-02-19
 *      Author: fred
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>

class Screen {

private:
	void setup_hardware_spi(void);
	void setup_timer1_ovf(void);

public:
	Screen(uint8_t clock, uint8_t latch, uint8_t data, uint8_t enable);
	void shiftLeft();
	void shiftRight();
	void doDraw();
	void drawSprite(uint8_t x,uint8_t y, uint8_t id);
	void clear();
	void plot(uint8_t x, uint8_t y, uint8_t val);
	void putchar_3x5(uint8_t x, uint8_t y, char c);
	void putchar_4x7(uint8_t x, uint8_t y, char c);

};

/** Singletons **/
//export Screen screen;

#endif
