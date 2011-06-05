/*

 Accelero 2011-02
 
 */

// features
#define ADC_ENABLE 1

#include "AnalogReader.h"
#include "Bounce.h"
#include "Screen.h"
#include "utils.h"

#include "WProgram.h" // arduino headers
#include "pins_arduino.h"

#include <avr/pgmspace.h>

// config
#if ADC_ENABLE
AnalogReader accx(2, -8.0, 8.0);
AnalogReader accy(3, -8.0, 8.0);
#endif
Screen screen; // clk, lat, dat, en
Bounce left = Bounce(2, 100);
Bounce right = Bounce(3, 100);

void setup() {
	randomSeed(555);
	analogReference( DEFAULT); // 5V

	// setup the input buttons
	pinMode(2, INPUT);
	pinMode(3, INPUT);

	sei();
}

int8_t count = 0;

uint32_t t2 = 0;
uint32_t t1 = 0, t3 = 0, t4 = 0;

void loop() {
	uint32_t now = millis();

	if (now - t4 > 10 /* ms */) {
		t4 = now;
		if (left.update()) {
			if (left.fallingEdge()) {
				count--;
				t2 = 0;
			}
		}
		if (right.update()) {
			if (right.fallingEdge()) {
				count++;
				t2 = 0;
			}
		}
	}

	if (now - t2 > 1000 /* ms */) {
		t2 = now;

#if 1
		// show count
		screen.clear();
		screen.setCursor(1, 1);
		screen << count;
#endif

#if 0
		// show display refresh rate
		screen.clear();
		screen.setCursor(1, 1);
		screen << screen.getRefreshRate();
#endif

#if 0
		// display line for left to right
		static uint8_t cx;

		screen.clear();
		for (uint8_t i = 0; i <= Y_MAX; i++) {
			screen.plot(cx, i, HIGH);
		}
		++cx &= X_MAX;
#endif

#if 0
		// shift to the left
		static uint8_t row;

		screen.shiftLeft();
		screen.plot(X_MAX - 1, row, LOW);
		++row &= Y_MAX;
		screen.plot(X_MAX - 1, row, HIGH);

#endif
	}

#if ADC_ENABLE
	static uint8_t lastrow, lastcol;
	if (now - t3 > (1000 / (200 /* Hz */))) {
		t3 = now;

		long ax = -accx.getLong();
		long ay = -accy.getLong();

#if 0
		// display x gforce value
		if (now - t1 > 20 /* ms */) {
			t1 = now;

			screen.clear();
			screen.setCursor(1, 1);
			screen << _FLOAT(ax/1000.0, 2) << endl;
		}
#endif

		long col = maplimit(ax, -1000, 1000, 0, X_MAX);
		long row = maplimit(ay, -1000, 1000, 0, Y_MAX);

		if (lastrow != row || lastcol != col) {
			screen.plot(lastcol, lastrow, LOW);
			lastrow = row;
			lastcol = col;
		}

	}
	screen.plot(lastcol, lastrow, HIGH);
#endif

}
