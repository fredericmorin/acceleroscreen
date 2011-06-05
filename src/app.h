/*
 * app.h
 *
 *  Created on: 2011-02-19
 *      Author: fred
 */

#ifndef APP_H_
#define APP_H_

#include "AnalogReader.h"
#include "utils.h"

#include "WProgram.h" // arduino headers
#include "pins_arduino.h"
#include <stdint.h>

#include <avr/pgmspace.h>

extern AnalogReader accx;
extern AnalogReader accy;
extern Screen screen;

class App {
private:
	int8_t count;
	uint32_t t2, t3;
	uint8_t lastrow, lastcol;

public:
	App() :
		count(0), t2(0), t3(0) {
	}

	inline void clickLeft() {

	}

	inline void clickRight() {

	}

	inline void updateAcc(int32_t& ax, int32_t& ay) {
		int8_t col = maplimit<int32_t>(ax, -1000, 1000, 0, X_MAX);
		int8_t row = maplimit<int32_t>(ay, -1000, 1000, 0, Y_MAX);

		if (lastrow != row || lastcol != col) {
			screen.plot(lastcol, lastrow, LOW);
			lastrow = row;
			lastcol = col;
		}
	}

	inline void updateScreen(uint32_t& now) {

		if (now - t2 > 100 /* ms */) {
			t2 = now;

#if 0
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

		screen.plot(lastcol, lastrow, HIGH);

	}

};

#endif // APP_H_
