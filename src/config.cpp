#include "config.h"
#include <Preferences.h>
static Preferences prefs;
bool loadConfig(FiberConfig &c){prefs.begin("fiberscout",true);c.ssid=prefs.getString("ssid",c.ssid);c.collector=prefs.getString("collector",c.collector);c.privacyNotice=prefs.getString("privacy",c.privacyNotice);c.neighborhood=prefs.getString("neigh",c.neighborhood);c.backendUrl=prefs.getString("url","");c.backendToken=prefs.getString("token","");prefs.end();return true;}
bool saveConfig(const FiberConfig &c){prefs.begin("fiberscout",false);bool ok=prefs.putString("ssid",c.ssid)&&prefs.putString("collector",c.collector)&&prefs.putString("privacy",c.privacyNotice)&&prefs.putString("neigh",c.neighborhood)&&prefs.putString("url",c.backendUrl)&&prefs.putString("token",c.backendToken);prefs.end();return ok;}
