#include "app.h"

// app
AppStartup appStartup;
AppMenu appMenu;
AppDot appDot;

App* currentApp;
App* app[APP_COUNT];

void init_apps() {
	app[APP_STARTUP] = &appStartup;
	app[APP_MENU] = &appMenu;
	app[APP_DOT] = &appDot;
}
