/*
 * appDot.h
 *
 *  Created on: 2011-06-05
 *      Author: fred
 */

#ifndef APPSTARTUP_H_
#define APPSTARTUP_H_

#include "app.h"

class AppStartup: public App {
private:
	uint32_t t2;
	uint32_t ref_time;
	bool firstUpdate;

public:
	AppStartup() {
	}

	virtual void load() {
		t2 = 0;
		firstUpdate = true;
		ref_time = millis();
		screen.clear();
	}

	virtual void updateScreen(uint32_t& now) {
		static const uint8_t anim_delay = 40;

		if (now - t2 > anim_delay /* ms */) {
			t2 = now;

			if (firstUpdate) {
				firstUpdate = false;

				screen.drawSprite(X_MAX - 8, 0, 0);
			} else {
				screen.shiftLeft();
			}
		}

		if (now > (ref_time + (anim_delay * (X_MAX + 2)))) {
			currentApp = app[APP_BAR];
			currentApp->load();
		}
	}
};

#endif // APPSTARTUP_H_
