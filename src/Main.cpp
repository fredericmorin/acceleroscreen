/*

 Accelero 2011-02
 
 */

#include "AnalogReader.h"
#include "Bounce.h"
#include "Screen.h"
#include "utils.h"
#include "app.h"

#include "WProgram.h" // arduino headers
#include "pins_arduino.h"
#include <inttypes.h>

#include <avr/pgmspace.h>

// config
AnalogReader accx(2, -8.0, 8.0);
AnalogReader accy(3, -8.0, 8.0);
Screen screen; // clk, lat, dat, en
Bounce left = Bounce(2, 100);
Bounce right = Bounce(3, 100);
App app;

void setup() {
	randomSeed(555);
	analogReference( DEFAULT); // 5V

	// setup the input buttons
	pinMode(2, INPUT);
	pinMode(3, INPUT);

	sei();
}


uint32_t t3 = 0, t4 = 0;

void loop() {
	uint32_t now = millis();

	if (now - t4 > 10 /* ms */) {
		t4 = now;
		if (left.update()) {
			if (left.fallingEdge()) {
				app.clickLeft();
			}
		}
		if (right.update()) {
			if (right.fallingEdge()) {
				app.clickRight();
			}
		}
	}

	if (now - t3 > (1000 / (100 /* Hz */))) {
		t3 = now;

		int32_t ax = -accx.getLong();
		int32_t ay = -accy.getLong();

		app.updateAcc(ax, ay);
	}


	app.updateScreen(now);

}
