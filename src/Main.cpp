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

// app
AppStartup appStartup;
AppMenu appMenu;
AppDot appDot;

App* currentApp;
App* app[3];

void setup() {
	randomSeed(555);
	analogReference( DEFAULT); // 5V

	// setup the input buttons
	pinMode(2, INPUT);
	pinMode(3, INPUT);

	// default app
	currentApp = &appStartup;
	app[0] = &appStartup;
	app[1] = &appMenu;
	app[2] = &appDot;

	sei();
}

uint32_t t3 = 0, t4 = 0;

void loop() {
	uint32_t now = millis();

	if (now - t4 > 50 /* ms */) {
		t4 = now;
		if (left.update()) {
			if (left.fallingEdge()) {
				currentApp->onLeftClick();
			}
		}
		if (right.update()) {
			if (right.fallingEdge()) {
				currentApp->onRightClick();
			}
		}
	}

	if (now - t3 > (1000 / (100 /* Hz */))) {
		t3 = now;

		int32_t ax = -accx.getLong();
		int32_t ay = -accy.getLong();

		currentApp->updateAccelValues(ax, ay);
	}

	currentApp->updateScreen(now);

	screen.swapBuffer();

}
