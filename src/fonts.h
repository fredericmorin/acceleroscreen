#include "binary.h"

#include <avr/pgmspace.h>

// warning fix
#ifdef PROGMEM
#undef PROGMEM
#endif
#define PROGMEM __attribute__((__section__(".text"))) // flash prog section

const PROGMEM uint8_t font_3x5[11][3] = { //
		//
				{ B11111, B10001, B11111 }, // 0
				{ B00000, B11111, B00000 }, // 1
				{ B10111, B10101, B11101 }, // 2
				{ B10001, B10101, B11111 }, // 3
				{ B11100, B00100, B11111 }, // 4
				{ B11101, B10101, B10111 }, // 5
				{ B11111, B10101, B10111 }, // 6
				{ B10000, B10000, B11111 }, // 7
				{ B11111, B10101, B11111 }, // 8
				{ B11100, B10100, B11111 }, // 9
				{ B00000, B00000, B00000 } // blank space
		};

const PROGMEM uint8_t font_4x7[11][4] = {
		{ B0111110, B1000001, B1000001, B0111110 }, // 0
		{ B0010000, B0100000, B1111111, B0000000 }, // 1
		{ B0100111, B1001001, B1001001, B0110011 }, // 2
		{ B0100010, B1001001, B1001001, B0110110 }, // 3
		{ B1111000, B0001000, B0001000, B1111111 }, // 4
		{ B1110010, B1001001, B1001001, B1000110 }, // 5
		{ B0111110, B1001001, B1001001, B0000110 }, // 6
		{ B1100000, B1000111, B1011000, B1100000 }, // 7
		{ B0110110, B1001001, B1001001, B0110110 }, // 8
		{ B0110000, B1001001, B1001001, B0111110 }, // 9
		{ B0, B0, B0, B0 } // blank space
};

// space invader sprites
const PROGMEM uint8_t sprite_00[] = { //
		//
				0x18, // ___XX___
				0x3C, // __XXXX__
				0x7E, // _XXXXXX_
				0xDB, // X_XXXX_X
				0xFF, // XXXXXXXX
				0x24, // __X__X__
				0x5A, // _X_XX_X_
				0xA5 //  X_X__X_X
		};

const PROGMEM uint8_t sprite_01[8] = { //
		//
				0x18, // ___XX___
				0x3C, // __XXXX__
				0x7E, // _XXXXXX_
				0xDB, // X_XXXX_X
				0xFF, // XXXXXXXX
				0x5A, // _X_XX_X_
				0x81, // X______X
				0x42 //  _X____X_
		};

const PROGMEM uint8_t sprite_02[8] = { //
		//
				B00100100, // __X__X__
				B00100100, // __X__X__
				B01111110, // _XXXXXX_
				B11011011, // XX_XX_XX
				B11111111, // XXXXXXXX
				B01111110, // _XXXXXX_
				B10000001, // X______X
				B01000010 //  _X____X_
		};

const PROGMEM uint8_t sprite_03[8] = { //
		//
				B01000010, // _X____X_
				B00100100, // __X__X__
				B01111110, // _XXXXXX_
				B11011011, // XX_XX_XX
				B11111111, // XXXXXXXX
				B00111100, // __XXXX__
				B00100100, // __X__X__
				B00011000 //  ___XX___
		};

// pacman "font"
const PROGMEM uint16_t pacman_12x12[3][12] = {
//
		// pacman open
		{ 0b00000000000, 0b00100000100, 0b01100000110, 0b01100000110,
				0b11010001111, 0b11111011111, 0b11111111111, 0b11111111111,
				0b01111111110, 0b01111111110, 0b00111111100, 0b00001110000 },
		// pacman half-closed
		{ 0b00010001000, 0b00111011100, 0b01111011110, 0b01111111110,
				0b11011111111, 0b11111111111, 0b11111111111, 0b11111111111,
				0b01111111110, 0b01111111110, 0b00111111100, 0b00001110000 },
		// pacman closed
		{ 0b00001110000, 0b00111111100, 0b01111111110, 0b01111111110,
				0b11011111111, 0b11111111111, 0b11111111111, 0b11111111111,
				0b01111111110, 0b01111111110, 0b00111111100, 0b00001110000 } };
