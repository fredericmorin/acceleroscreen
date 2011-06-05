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
	static const uint8_t section_last = 6;

	int8_t section_id;
	uint32_t last_update;

public:
	AppMenu() {
		section_id = 0;
		last_update = 0;
	}

	virtual void onLeftClick() {
		section_id++;
		if (section_id > section_last)
			section_id = 0;
		last_update = 0;
	}

	virtual void onRightClick() {
		screen.clear();
		last_update = 0;
	}

	virtual void updateScreen(uint32_t& now) {
		if (now - last_update > 100 /* ms */) {
			last_update = now;

			screen.clear();
			for (uint8_t i = 0; i <= section_last; i++) {
				screen.plot(0, i, HIGH);
			}
			screen.plot(1, section_id, HIGH);
			screen.setCursor(4, 1);

			switch (section_id) {
			case 1: {
				break;
			}
			default:
				screen << section_id;
			}
		}
	}

};

#endif // APPMENU_H_
