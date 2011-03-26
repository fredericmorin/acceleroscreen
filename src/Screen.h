/*
 * screen.h
 *
 *  Created on: 2011-02-19
 *      Author: fred
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>

#include "Print.h"
#include "Streaming.h"

// Screen config
#define panel_count_x (4)
#define panel_count_y (1)
#define panel_count (panel_count_x * panel_count_y)
#define ROWS_PER_PANEL (8)
#define X_MAX (8 * panel_count_x - 1) // horizontal display size
#define Y_MAX (8 * panel_count_y - 1) // vertical display size

class Screen: public Print {

private:

	uint8_t posx, posy;

	void setup_hardware_spi(void);
	void setup_timer1_ovf(void);

	void putchar_3x5(uint8_t x, uint8_t y, uint8_t c);
	void putchar_4x7(uint8_t x, uint8_t y, uint8_t c);

public:
	Screen();

	virtual void write(uint8_t);

	void clear();

	void setCursor(uint8_t posx, uint8_t posy);

	void shiftLeft();
	void shiftRight();

	void drawSprite(uint8_t x, uint8_t y, uint8_t id);

	void plot(uint8_t x, uint8_t y, uint8_t val);

};

#endif
