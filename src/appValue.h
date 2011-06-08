/*
 * appValue.h
 *
 *  Created on: 2011-06-07
 *      Author: fred
 */

#ifndef APPVALUE_H_
#define APPVALUE_H_

#include "app.h"

class AppValue: public App {
private:
	uint32_t last_update;
	int32_t ax, ay;

public:
	AppValue() {
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
		currentApp = app[APP_DOT];
		currentApp->load();
	}

	virtual void updateAccelValues(int32_t& ax_, int32_t& ay_) {
		ax = ax_;
		ay = ay_;
	}

	virtual void updateScreen(uint32_t& now) {
		if (now - last_update > 50 /* ms */) {
			last_update = now;

			screen.clear();
			if (ax < 0)
				screen.setCursor(1, 1);
			else
				screen.setCursor(5, 1);
			screen << _FLOAT(ax / 1000.0f, 2) << " g";
		}
	}

};

#endif // APPVALUE_H_
