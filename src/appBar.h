/*
 * appHist.h
 *
 *  Created on: 2011-06-07
 *      Author: fred
 */

#ifndef APP_BAR_H_
#define APP_BAR_H_

#include "app.h"

class AppBar: public App {
private:
	enum {
		X_AXIS,
		Y_AXIS
	} mode;
	uint8_t ypos, xpos;
	int32_t ax, ay;

public:
	bool fill;

	AppBar() {
		fill = false;
	}

	virtual void load() {
		mode = X_AXIS;
		screen.clear();
	}

	virtual void onLeftClick() {
		currentApp = app[APP_MENU];
		currentApp->load();
	}

	virtual void onRightClick() {
		if (mode == X_AXIS)
			mode = Y_AXIS;
		else
			mode = X_AXIS;
	}

	virtual void updateAccelValues(int32_t& ax, int32_t& ay) {
		xpos = maplimit<int32_t>(ax, -1000, 1000, 0, X_MAX);
		ypos = maplimit<int32_t>(-ay, -1000, 1000, 0, X_MAX);
	}

	virtual void updateScreen(uint32_t& now) {
		screen.clear();
		screen.setCursor(1, 1);
		screen.write('x' + mode);
		screen.plot(X_MAX / 2, 0, HIGH);
		screen.plot(X_MAX / 2, 1, HIGH);
		screen.plot(X_MAX / 2, Y_MAX - 1, HIGH);
		screen.plot(X_MAX / 2, Y_MAX, HIGH);
		uint8_t pos = (mode == X_AXIS ? xpos : ypos);
		if (fill) {
			uint8_t begin = ((X_MAX / 2) <= pos ? (X_MAX / 2) : pos);
			uint8_t end = ((X_MAX / 2) >= pos ? (X_MAX / 2) : pos);
			for (uint8_t i = begin; i <= end; i++)
				screen.line(i, 0, i, Y_MAX);
		} else {
			screen.line(pos, 0, pos, Y_MAX);
		}
	}

};

#endif // APP_BAR_H_
