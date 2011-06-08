/*
 * app.h
 *
 *  Created on: 2011-06-05
 *      Author: fred
 */

#ifndef APP_H_
#define APP_H_

#include "Screen.h"
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
public:
	virtual void load() {}
	virtual void onLeftClick() {}
	virtual void onRightClick() {}
	virtual void updateAccelValues(int32_t& ax, int32_t& ay) {}
	virtual void updateScreen(uint32_t& now) {}
};

extern App* currentApp;
extern App* app[];
enum {
	APP_STARTUP = 0,
	APP_MENU,
	APP_DOT,
	APP_VALUE,
	APP_HIST,
	//
	APP_COUNT
};

#include "appStartup.h"
#include "appMenu.h"
#include "appDot.h"
#include "appValue.h"
#include "appHist.h"

extern void init_apps();

#endif // APP_H_
