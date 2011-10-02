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
#define PANEL_COUNT_X (4)
#define PANEL_COUNT_Y (1)
#define PANEL_COUNT (PANEL_COUNT_X * PANEL_COUNT_Y)
#define COLS_PER_PANEL (8)
#define ROWS_PER_PANEL (8)
#define X_MAX (COLS_PER_PANEL * PANEL_COUNT_X - 1) // horizontal display size
#define Y_MAX (ROWS_PER_PANEL * PANEL_COUNT_Y - 1) // vertical display size

class Screen: public Print {

private:

	uint8_t posx, posy;

	void setup_hardware_spi(void);
	void setup_timer1_ovf(void);

	bool putchar_3x5(uint8_t x, uint8_t y, uint8_t c);
	bool putchar_4x7(uint8_t x, uint8_t y, uint8_t c);

	void plot_(uint8_t x, uint8_t y, uint8_t val);

public:
	Screen();

	void swapBuffer();

	virtual void write(uint8_t);

	void clear();

	void setCursor(uint8_t posx, uint8_t posy);

	void shiftLeft();
	void shiftRight();

	void drawSprite(uint8_t x, uint8_t y, uint8_t id);

	void plot(uint8_t x, uint8_t y, uint8_t val);

	void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

	uint16_t getRefreshRate();

};

#endif
