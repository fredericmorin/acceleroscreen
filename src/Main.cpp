/*

 Accelero 2011-02
 
 */

// features
#define LCDDISPLAY 0
#define ADC_ENABLE 1

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
#define ACCEL_READ_RATE    200 // Hz
AnalogReader accx(2, -8.0, 8.0);
AnalogReader accy(1, -8.0, 8.0);
#endif
#if 0
AnalogReader batt(0, 0, 15.6);
#endif
Screen screen(SCK, SS, MOSI, 9); // clk, lat, dat, en

void setup() {
	randomSeed(555);
	analogReference( DEFAULT); // 5V

	sei();
	screen.setCursor(1, 1);
	screen << "allo" << endl;
	delay(200);
	screen.shiftLeft();
	delay(50);
	screen.shiftLeft();
	delay(50);
	screen.shiftLeft();
	delay(50);
	screen.shiftLeft();
	delay(50);
	screen.shiftLeft();
	delay(50);
	screen.shiftLeft();
	delay(50);
	screen.shiftLeft();
	delay(50);
	screen.clear();
	screen.setCursor(1, 1);
	screen << "ohhh" << endl;

#if LCDDISPLAY
	lcd.begin(20, 4);
	lcd.print("ready");
#endif
}

uint32_t t1 = 0, t2 = 0, t3 = 0;

uint16_t loop_exec = 0;
void loop() {
	uint32_t now = millis();

	screen.shiftRight();

#if 0
	if (now - t1 > 2000 /* ms */) {
		t1 = now;

	}

	if (now - t2 > 200 /* ms */) {
		t2 = now;

		// screen.shiftLeft();
	}
#endif

#if ADC_ENABLE
	static uint8_t lastrow, lastcol;
	if (now - t3 > (1000 / ACCEL_READ_RATE /* Hz */)) {
		t3 = now;

		long col = maplimit(-accy.getInt(), -1000, 1000, 0, X_MAX);
		long row = maplimit(accx.getInt(), -1000, 1000, 0, Y_MAX);

		if (lastrow != row || lastcol != col) {
			screen.plot(lastcol, lastrow, 0);
			lastrow = row;
			lastcol = col;
		}

	}
	screen.plot(lastcol, lastrow, 1);
#endif

#if LCDDISPLAY
	if (now - t2 > (1000 / LCDDISPLAY_REFRESH_RATE /* Hz */)) {
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

