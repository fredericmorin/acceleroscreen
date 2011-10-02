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
	bool sensible;

public:
	AppHist() {
		last_update = 0;
		sensible = false;
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
		sensible = !sensible;
		screen.clear();
	}

	virtual void updateAccelValues(int32_t& ax_, int32_t& ay_) {
		ax = ax_;
		ay = ay_;
	}

	virtual void updateScreen(uint32_t& now) {
		if (now - last_update > 50 /* ms */) {
			last_update = now;

			screen.shiftLeft();

			if (sensible) {
				int8_t row = maplimit<int32_t> (ax, -500, 500, 0, Y_MAX);
				screen.plot(X_MAX, row, HIGH);
			} else {
				int8_t row = maplimit<int32_t> (ax, -1000, 1000, 0, Y_MAX);
				screen.plot(X_MAX, row, HIGH);
			}

			screen.plot(0, sensible ? 0 : 1, HIGH);
		}
	}

};

#endif // APP_HIST_H_
