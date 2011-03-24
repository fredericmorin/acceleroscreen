/*
 * screen.cpp
 *
 *  Created on: 2011-02-19
 *      Author: fred
 */

#include "Screen.h"

#include "WProgram.h"

#include "config.h"
#include "fonts.h"
#include "utils.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#ERROR not supported
#else
#define __SPI_LATCH_LOW   PORTB &= ~(1 << 2) // 1 -> 10 -> latch
#define __SPI_LATCH_HIGH  PORTB |=  (1 << 2)
#endif

#define SHADOW_SIZE (8 * panel_count) // 8 row per panel, 1 byte per row
volatile uint8_t shadowram[SHADOW_SIZE]; // our copy of the display's RAM

Screen::Screen(uint8_t clock, uint8_t latch, uint8_t data, uint8_t enable) : //
	posx(0), posy(0), clear_on_next_char(0) {

	// 13, 10, 11, 9
	pinMode(latch, OUTPUT);
	pinMode(clock, OUTPUT);
	pinMode(data, OUTPUT);
	pinMode(enable, OUTPUT);

	digitalWrite(enable, LOW); // active low
	__SPI_LATCH_HIGH;

	clear();

	setup_hardware_spi();
	_delay_ms(20);
	setup_timer1_ovf();

}

void Screen::setPos(uint8_t posx, uint8_t posy) {
	this->posx = posx;
	this->posy = posy;
}

void Screen::write(uint8_t c) {
	if (c == '\n') {
		clear_on_next_char = 1;
	}
	if (((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) //
			|| (c >= '0' && c <= '9') //
			|| c == ' ') {
		if (clear_on_next_char) {
			clear_on_next_char = 0;
			posx = 0;
			clear();
		}
		putchar_3x5(posx, posy, c);
		posx += 4;
	}
}

void Screen::shiftLeft() {
	for (uint8_t panel_x = 0; panel_x < panel_count_x; panel_x++) {
		if (panel_x == 0) {
			for (uint8_t row = 0; row < ROWS; row++) {
				shadowram[row] >>= 1;
			}
		} else {
			uint8_t offset = panel_x * 8;
			for (uint8_t row = 0; row < ROWS; row++) {
				shadowram[offset - 8 + row] |= //
						(shadowram[offset + row] & B00000001) << 7;
				shadowram[offset + row] >>= 1;
			}
		}
	}

}

void Screen::shiftRight() {
	// TODO
}

void Screen::plot(uint8_t x, uint8_t y, uint8_t val) {
	// exit if x or y is out of bounds
	if ((x < 0) || (y < 0) || (x > X_MAX) || (y > Y_MAX)) {
		return;
	}

	uint8_t addr, bitval;

	bitval = 1 << (x & B00000111);
	addr = 8 * (x >> 3) + y;

	if (val) { // Modify the shadow memory
		shadowram[addr] |= bitval;
	} else {
		shadowram[addr] &= ~bitval;
	}
}

void Screen::clear() {
	uint8_t i;
	for (i = 0; i < SHADOW_SIZE; i++)
		shadowram[i] = 0;
}

void Screen::putchar_3x5(uint8_t x, uint8_t y, uint8_t c) {
	uint8_t dots;
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
		c &= 0x1F; // A-Z maps to 1-26
	} else if (c >= '0' && c <= '9') {
		c = (c - '0') + 27;
	} else if (c == ' ') {
		c = 10; // space
	}
	for (uint8_t col = 0; col < 3; col++) {
		dots = pgm_read_byte_near(&font_3x5[c][col]);
		for (uint8_t row = 0; row < 5; row++) {
			if (dots & (16 >> row)) // only 5 rows.
				plot(x + col, y + row, 1);
			else
				plot(x + col, y + row, 0);
		}
	}
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

void Screen::putchar_4x7(byte x, byte y, uint8_t c) {
	uint8_t dots;
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
		c &= 0x1F; // A-Z maps to 1-26
	} else if (c >= '0' && c <= '9') {
		//c = (c - '0') + 27;
		c = (c - '0');
	} else if (c == ' ') {
		c = 10; // space
	}
	for (uint8_t col = 0; col < 4; col++) {
		dots = pgm_read_byte_near(&font_4x7[c][col]);
		for (uint8_t row = 0; row < 7; row++) {
			if (dots & (64 >> row)) // only 7 rows.
				plot(x + col, y + row, 1);
			else
				plot(x + col, y + row, 0);
		}
	}
}

void Screen::setup_hardware_spi(void) {
	byte clr;
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
	SPCR |= ((1 << SPE) | (1 << MSTR)); // enable SPI as master
	//SPCR |= ( (1<<SPR1) ); // set prescaler bits
	SPCR &= ~((1 << SPR1) | (1 << SPR0)); // clear prescaler bits
	clr = SPSR; // clear SPI status reg
	clr = SPDR; // clear SPI data reg
	SPSR |= (1 << SPI2X); // set prescaler bits
	//SPSR &= ~(1<<SPI2X); // clear prescaler bits
}

// display refresh rate
#define __TIMER1_MAX 0xFFFF // 16 bit CTR
volatile uint32_t timer1_precharge = 0x60;
volatile uint32_t timer1_timer = 0;
volatile uint16_t timer1_exec = 0;

void Screen::setup_timer1_ovf(void) {
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
	TCCR1B &= ~((1 << CS11));
	TCCR1B |= ((1 << CS12) | (1 << CS10));
	//normal mode
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	//Timer1 Overflow Interrupt Enable
	TIMSK1 |= (1 << TOIE1);
	TCNT1 = __TIMER1_MAX - timer1_precharge;
	// enable all interrupts
	sei();
}

ISR(TIMER1_OVF_vect) {
	uint8_t oldSREG = SREG;
	cli();

	// reset timer
	TCNT1 = __TIMER1_MAX - timer1_precharge;

	static uint8_t row;

	__SPI_LATCH_LOW;
	for (uint8_t panel = 0; panel < panel_count; panel++) {
		spi_transfer(~shadowram[row + panel * 8]);
		spi_transfer((1 << row));
	}
	__SPI_LATCH_HIGH;

	row++; // next time the ISR runs, the next row will be dealt with
	if (row >= ROWS) {
		row = 0;
	}

	SREG = oldSREG;
}

