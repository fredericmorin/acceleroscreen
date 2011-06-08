/*
 * appHist.h
 *
 *  Created on: 2011-06-07
 *      Author: fred
 */

#ifndef APP_HIST_H_
#define APP_HIST_H_

#include "app.h"

class AppHist: public App {
private:
	uint32_t last_update;
	int32_t ax, ay;

public:
	AppHist() {
		last_update = 0;
	}

	virtual void load() {
		last_update = 0;
		screen.clear();
	}

	virtual void onLeftClick() {
		currentApp = app[APP_MENU];
		currentApp->load();
	}

	virtual void onRightClick() {
		currentApp = app[APP_MENU];
		currentApp->load();
	}

	virtual void updateAccelValues(int32_t& ax_, int32_t& ay_) {
		ax = ax_;
		ay = ay_;
	}

	virtual void updateScreen(uint32_t& now) {
		if (now - last_update > 50 /* ms */) {
			last_update = now;

			screen.shiftLeft();

			int8_t row = maplimit<int32_t> (ax, -1000, 1000, 0, Y_MAX);
			screen.plot(X_MAX - 3, row, HIGH);
		}
	}

};

#endif // APP_HIST_H_
