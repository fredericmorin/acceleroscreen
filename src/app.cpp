#include "app.h"

// app
AppStartup appStartup;
AppMenu appMenu;
AppDot appDot;
AppValue appValue;
AppHist appHist;

App* currentApp;
App* app[APP_COUNT];

void init_apps() {
	app[APP_STARTUP] = &appStartup;
	app[APP_MENU] = &appMenu;
	app[APP_DOT] = &appDot;
	app[APP_VALUE] = &appValue;
	app[APP_HIST] = &appHist;
}
