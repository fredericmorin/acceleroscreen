/*
 * appDot.h
 *
 *  Created on: 2011-02-19
 *      Author: fred
 */

#ifndef APPSTARTUP_H_
#define APPSTARTUP_H_

#include "app.h"

class AppStartup: public App {
private:
	uint32_t t2;
	bool firstUpdate;

public:
	AppStartup() {
		t2 = 0;
		firstUpdate = true;
	}

	virtual void updateScreen(uint32_t& now) {
		if (now - t2 > 40 /* ms */) {
			t2 = now;

			if (firstUpdate) {
				firstUpdate = false;

				screen.drawSprite(X_MAX - 8, 0, 0);
			} else {
				screen.shiftLeft();
			}
		}

		if (now > 2000) {
			screen.clear();
			currentApp = app[1];
		}
	}
};

#endif // APPSTARTUP_H_
