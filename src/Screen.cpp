/*
 * screen.cpp
 *
 *  Created on: 2011-02-19
 *      Author: fred
 */

/*
 * ** TODO **
 * - double buffering
 * - variable pixel intensity
 */

#include "Screen.h"

#include "WProgram.h"
#include "pins_arduino.h"

#include "fonts.h"
#include "utils.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#define __SPI_LATCH_LOW   PORTB &= ~_BV(PB2) // effective when low, arduino pin 10
#define __SPI_LATCH_HIGH  PORTB |=  _BV(PB2)
#define __SCREEN_ENABLE   PORTB &= ~_BV(PB3) // enabled when low, arduino pin 11
#define __SCREEN_DISABLE  PORTB |=  _BV(PB3)

/**
 * in memory panel organisation
 * -----------------
 * | 1 | 2 | 3 | 4 |
 * -----------------
 * | 5 | 6 | 7 | 8 |
 * -----------------
 */
struct panel {
	uint8_t row[ROWS_PER_PANEL];
};
enum {
	READ_BUF = 0,
	WRITE_BUF,
	BUF_COUNT
};
panel screen_buffer[BUF_COUNT][PANEL_COUNT];

Screen::Screen() : //
	posx(0), posy(0) {

	pinMode(SCK, OUTPUT); // clock
	pinMode(MOSI, OUTPUT); // data out
	pinMode(SS, OUTPUT); // latch
	pinMode(12, OUTPUT); // enable

	__SCREEN_ENABLE;
	__SPI_LATCH_HIGH; // unactive when high

	clear();

	setup_hardware_spi();
	_delay_ms(20);
	setup_timer1_ovf();
}

inline void Screen::plot_(uint8_t x, uint8_t y, uint8_t val) {
	uint8_t panel, row, bitval;

	bitval = 1 << (x & B00000111);
	panel = (x >> 3) + (y >> 3) * PANEL_COUNT_X;
	row = (y & B00000111);

	if (val) { // Modify the shadow memory
		screen_buffer[WRITE_BUF][panel].row[row] |= bitval;
	} else {
		screen_buffer[WRITE_BUF][panel].row[row] &= ~bitval;
	}
}

void Screen::clear() {
	memset(screen_buffer[WRITE_BUF], 0, sizeof(panel) * PANEL_COUNT);
}

void Screen::swapBuffer() {
	memcpy(screen_buffer[READ_BUF], screen_buffer[WRITE_BUF], sizeof(panel) * PANEL_COUNT);
}

/* mid level stuff */

void Screen::plot(uint8_t x, uint8_t y, uint8_t val) {
	// exit if x or y is out of bounds
	if ((x < 0) || (y < 0) || (x > X_MAX) || (y > Y_MAX)) {
		return;
	}

	plot_(x, y, val);
}

void Screen::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	if ((x0 < 0) || (y0 < 0) || (x0 > X_MAX) || (y0 > Y_MAX) || (x1 < 0) || (y1
			< 0) || (x1 > X_MAX) || (y1 > Y_MAX)) {
		return;
	}

	uint8_t dx = abs(x1 - x0);
	uint8_t dy = abs(y1 - y0);
	int8_t sx, sy;
	if (x0 < x1)
		sx = 1;
	else
		sx = -1;
	if (y0 < y1)
		sy = 1;
	else
		sy = -1;
	int8_t err = dx - dy;

	while (1) {
		plot_(x0, y0, 1);
		if (x0 == x1 && y0 == y1)
			break;
		int8_t e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x0 = x0 + sx;
		}
		if (e2 < dx) {
			err = err + dx;
			y0 = y0 + sy;
		}
	}
}

void Screen::shiftLeft() {
	for (uint8_t panel_y = 0; panel_y < PANEL_COUNT_Y; panel_y++) {
		for (uint8_t panel_x = 0; panel_x < PANEL_COUNT_X; panel_x++) {
			uint8_t panel = panel_x + PANEL_COUNT_X * panel_y;
			if (panel_x == 0) {
				for (uint8_t row = 0; row < ROWS_PER_PANEL; row++) {
					screen_buffer[WRITE_BUF][panel].row[row] >>= 1;
				}
			} else {
				for (uint8_t row = 0; row < ROWS_PER_PANEL; row++) {
					screen_buffer[WRITE_BUF][panel - 1].row[row] |= //
							(screen_buffer[WRITE_BUF][panel].row[row] & B00000001) << 7;
					screen_buffer[WRITE_BUF][panel].row[row] >>= 1;
				}
			}
		}
	}
}

void Screen::shiftRight() {
	// TODO
}

/* high level stuff */

void Screen::setCursor(uint8_t posx, uint8_t posy) {
	this->posx = posx;
	this->posy = posy;
}

void Screen::write(uint8_t c) {
	uint8_t width = putchar_3x5(posx, posy, c);
	posx += width + 1;
}

uint8_t Screen::putchar_3x5(uint8_t x, uint8_t y, uint8_t c) {
	uint8_t width = 3;
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
		c &= 0x1F; // A-Z maps to 1-26
	} else if (c >= '0' && c <= '9') {
		c = (c - '0') + 27;
	} else if (c == ' ') {
		return 2;
		c = 0; // space
	} else if (c == '.') {
		width = 1;
		c = 37;
	} else if (c == '-') {
		width = 2;
		c = 38;
	} else if (c == ':') {
		width = 1;
		c = 39;
	} else {
		return 0;
	}
	uint8_t dots;
	for (uint8_t col = 0; col < 3; col++) {
		dots = pgm_read_byte_near(&font_3x5[c][col]);
		for (uint8_t row = 0; row < 5; row++) {
			if (dots & (16 >> row)) // only 5 rows.
				plot(x + col, y + row, 1);
			else
				plot(x + col, y + row, 0);
		}
	}
	return width;
}

uint8_t Screen::putchar_4x7(byte x, byte y, uint8_t c) {
	uint8_t width = 3;
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
		c &= 0x1F; // A-Z maps to 1-26
	} else if (c >= '0' && c <= '9') {
		//c = (c - '0') + 27;
		c = (c - '0');
	} else if (c == ' ') {
		c = 10; // space
	} else {
		return 0;
	}
	uint8_t dots;
	for (uint8_t col = 0; col < 4; col++) {
		dots = pgm_read_byte_near(&font_4x7[c][col]);
		for (uint8_t row = 0; row < 7; row++) {
			if (dots & (64 >> row)) // only 7 rows.
				plot(x + col, y + row, 1);
			else
				plot(x + col, y + row, 0);
		}
	}
	return width;
}

void Screen::drawSprite(uint8_t x, uint8_t y, uint8_t id) {
	const uint8_t* sprite;
	if (id == 0) {
		sprite = sprite_00;
	} else if (id == 1) {
		sprite = sprite_01;
	} else if (id == 2) {
		sprite = sprite_02;
	} else if (id == 3) {
		sprite = sprite_03;
	} else {
		return;
	}
	uint8_t dots;
	for (uint8_t row = 0; row < 8; row++) {
		dots = pgm_read_byte_near(&sprite[row]);
		for (uint8_t col = 0; col < 8; col++) {
			if (dots & (1 << col)) // only 7 rows.
				plot(x + col, y + row, 1);
			else
				plot(x + col, y + row, 0);
		}
	}
}

/* initialisation stuff */

inline void Screen::setup_hardware_spi(void) {
	uint8_t clr;
	// spi prescaler:
	// SPI2X SPR1 SPR0
	//   0     0     0    fosc/4
	//   0     0     1    fosc/16
	//   0     1     0    fosc/64
	//   0     1     1    fosc/128
	//   1     0     0    fosc/2
	//   1     0     1    fosc/8
	//   1     1     0    fosc/32
	//   1     1     1    fosc/64

	/* enable SPI as master */
	SPCR |= (_BV(SPE) | _BV(MSTR));
	/* clear registers */
	clr = SPSR;
	clr = SPDR;
	/* set prescaler to fosc/2 */
	SPCR &= ~(_BV(SPR1) | _BV(SPR0));
	SPSR |= _BV(SPI2X);
}

inline void Screen::setup_timer1_ovf(void) {
	// Arduino runs at 16 Mhz...
	// Timer1 (16bit) Settings:
	// prescaler (frequency divider) values:
	// CS12    CS11   CS10
	//   0       0      0    stopped
	//   0       0      1      /1
	//   0       1      0      /8
	//   0       1      1      /64
	//   1       0      0      /256
	//   1       0      1      /1024
	//   1       1      0      external clock on T1 pin, falling edge
	//   1       1      1      external clock on T1 pin, rising edge
	//
	uint8_t _sreg = SREG; /* save SREG */
	cli(); /* disable all interrupts while messing with the register setup */

	/* multiplexed TRUE-RGB PWM mode (quite dim) */
	/* set prescaler to 256 */
	TCCR1B |= (_BV(CS12));
	TCCR1B &= ~(_BV(CS10) | _BV(CS11));
	/* set WGM mode 4: CTC using OCR1A */
	TCCR1A &= ~(_BV(WGM10) | _BV(WGM11));
	TCCR1B |= _BV(WGM12);
	TCCR1B &= ~_BV(WGM13);
	/* normal operation - disconnect PWM pins */
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));
	/* set top value for TCNT1 */
#define __TRUE_RGB_OCR1A 0x0088
	OCR1A = __TRUE_RGB_OCR1A;
	/* enable COMPA isr */
	TIMSK1 |= _BV(OCIE1A);
	/* restore SREG with global interrupt flag */
	SREG = _sreg;
}

volatile uint16_t refreshcnt = 0;

uint16_t Screen::getRefreshRate() {
	uint16_t val = refreshcnt;
	refreshcnt = 0;
	return val;
}

//#define __TIMER1_MAX 0xFFFF // 16 bit CTR
//#define __TIMER1_PRECHARGE 0x010;

ISR(TIMER1_COMPA_vect) {
	// reset timer
	//TCNT1 = __TIMER1_MAX - __TIMER1_PRECHARGE;

	//__SCREEN_ENABLE;

	static uint8_t row;

	__SPI_LATCH_LOW;
	for (uint8_t panel = 0; panel < PANEL_COUNT; panel++) {
		spi_transfer(~screen_buffer[READ_BUF][panel].row[row]);
		spi_transfer((1 << row));
	}
	__SPI_LATCH_HIGH;

	row++; // deal with the next row next time the ISR runs
	if (row >= ROWS_PER_PANEL) {
		row = 0;
		if (refreshcnt ^= 0xFFFF) {
			refreshcnt++;
		}
	}

	//_delay_us(30);
	//__SCREEN_DISABLE;

}
