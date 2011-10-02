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
	uint8_t lastrow, lastcol;

public:
	AppDot() {
	}

	virtual void load() {
		screen.clear();
	}

	virtual void onLeftClick() {
		currentApp = app[APP_MENU];
		currentApp->load();
	}

	virtual void onRightClick() {

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
		screen.plot(X_MAX / 2, 0, HIGH);
		screen.plot(X_MAX / 2, Y_MAX, HIGH);
		screen.plot(lastcol, lastrow, HIGH);
	}

};

#endif // APPDOT_H_
