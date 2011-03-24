/*

 Accelero 2011-02
 
 */

// features
#define LCDDISPLAY 0
#define ADC_ENABLE 0

#include "config.h"

#include "AnalogReader.h"
#include "Screen.h"
#include "utils.h"

#include "WProgram.h" // arduino headers
#include "pins_arduino.h"

#include <avr/pgmspace.h>

#if LCDDISPLAY
#define LCDDISPLAY_REFRESH_RATE    10 // Hz
#define ARDUINO 22
#include "Streaming.h"
#include "LiquidCrystal.h"
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
#endif

// config
#if ADC_ENABLE
#define ACCEL_READ_RATE    100 // Hz
AnalogReader accx(7, -8.0, 8.0);
AnalogReader accy(6, -8.0, 8.0);
AnalogReader batt(0, 0, 15.6);
#endif
Screen screen(SCK, SS, MOSI, 9); // clk, lat, dat, en

void setup() {
	randomSeed(555);
	analogReference( DEFAULT); // 5V

#if LCDDISPLAY
	lcd.begin(20, 4);
	lcd.print("ready");
#endif
}

uint32_t t1 = 0, t2 = 0, t3 = 0;
#if LCDDISPLAY
const uint32_t t2out = 1000 / LCDDISPLAY_REFRESH_RATE;
#endif
#if ADC_ENABLE
const uint32_t t3out = 1000 / ACCEL_READ_RATE;
uint8_t lastrow = 0xFF, lastcol = 0xFF;
#endif

const uint32_t t1out = 2000; //ms
const uint32_t t2out = 200; //ms

uint16_t loop_exec = 0;
void loop() {
	uint32_t now = millis();

	if (now - t1 > t1out) {
		t1 = now;

		static byte a;

		//screen.clear();
		//screen.putchar_4x7(11, 0, car);
		if (a == 4)
			screen.putchar_3x5(X_MAX - 8, 0, '2');
		else if (a == 5)
			screen.putchar_4x7(X_MAX - 8, 0, '8');
		else
			screen.drawSprite(X_MAX - 8, 0, a);

		if (++a >= 6)
			a = 0;

	}

	if (now - t2 > t2out) {
		t2 = now;

		screen.shiftLeft();

	}

#if ADC_ENABLE
	if (now - t3 > t3out) {
		t3 = now;

		long row = maplimit(-accx.getInt(), -800, 800, 0, __rows - 1);
		long col = maplimit(-accy.getInt(), -800, 800, 0, __cols - 1);

		if (lastrow != row || lastcol != col) {
			screen.plot(lastrow, lastcol, 0);
			lastrow = row;
			lastcol = col;
		}

	}
	screen.plot(lastrow, lastcol, 1);
#endif

#if LCDDISPLAY
	if (now - t2 > t2out) {
		t2 = now;

		/*
		 */
		lcd.setCursor(0, 0);
		lcd << "x:" << _FLOAT(-accx.get(), 2) << " y:"
		<< _FLOAT(-accy.get(), 2) << "  ";

		lcd.setCursor(13, 2);
		lcd << _FLOAT(batt.get(), 2) << "v ";

		static uint8_t t2ctn;
		t2ctn++;
		if (t2ctn % 2 == 0) {
			const uint32_t lo_exec = loop_exec * 1000UL / t2out;
			lcd.setCursor(0, 2);
			lcd << _DEC(lo_exec) << " ";
			t2ctn = 0;
		}
		loop_exec = 0;

	}

	loop_exec++;
#endif

}

