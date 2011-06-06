/*
 * appDot.h
 *
 *  Created on: 2011-02-19
 *      Author: fred
 */

#ifndef APPMENU_H_
#define APPMENU_H_

#include "app.h"

class AppMenu: public App {
private:
	static const uint8_t section_last = 5;

	int8_t section_id;
	uint32_t last_update;

public:
	AppMenu() {
		section_id = 0;
	}

	virtual void load() {
		last_update = 0;
		screen.clear();
	}

	virtual void onLeftClick() {
		section_id++;
		if (section_id > section_last)
			section_id = 0;
		last_update = 0;
	}

	virtual void onRightClick() {
		switch (section_id) {
		case 0: {
			currentApp = app[APP_DOT];
			currentApp->load();
			break;
		}
		case section_last: {
			currentApp = app[APP_STARTUP];
			currentApp->load();
			break;
		}
		}
	}

	virtual void updateScreen(uint32_t& now) {
		if (now - last_update > 100 /* ms */) {
			last_update = now;

			screen.clear();
			for (uint8_t i = 0; i <= section_last; i++) {
				screen.plot(0, i, HIGH);
			}
			screen.plot(1, section_id, HIGH);
			screen.setCursor(3, 1);
			screen << section_id;
			screen.setCursor(9, 1);

			switch (section_id) {
			case 0: {
				screen << "dot";
				break;
			}
			}
		}
	}

};

#endif // APPMENU_H_
