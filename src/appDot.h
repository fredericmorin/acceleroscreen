/*
 * appDot.h
 *
 *  Created on: 2011-06-05
 *      Author: fred
 */

#ifndef APPDOT_H_
#define APPDOT_H_

#include "app.h"

class AppDot : public App {
private:
	uint32_t t2;
	uint8_t lastrow, lastcol;

public:
	AppDot() {
		t2 = 0;
	}

	virtual void load() {
		t2 = 0;
		screen.clear();
	}

	virtual void onLeftClick() {
		currentApp = app[APP_MENU];
		currentApp->load();
	}

	virtual void onRightClick() {
		currentApp = app[APP_VALUE];
		currentApp->load();
	}

	virtual void updateAccelValues(int32_t& ax, int32_t& ay) {
		int8_t col = maplimit<int32_t>(ax, -1000, 1000, 0, X_MAX);
		int8_t row = maplimit<int32_t>(ay, -1000, 1000, 0, Y_MAX);

		if (lastrow != row || lastcol != col) {
			screen.plot(lastcol, lastrow, LOW);
			lastrow = row;
			lastcol = col;
		}
	}

	virtual void updateScreen(uint32_t& now) {
		screen.plot(lastcol, lastrow, HIGH);
	}

};

#endif // APPDOT_H_
