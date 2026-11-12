#include <Arduino.h>
#include <WiFi.h>
#include <M5Cardputer.h>
#include "config.h"
#include "lead_store.h"
#include "session.h"
#include "web_portal.h"
#include "display_ui.h"
#include "sync_queue.h"
FiberConfig config;
void setup(){Serial.begin(115200);delay(250);loadConfig(config);M5Cardputer.begin();uiBegin();if(!initLeadStore())M5Cardputer.Display.println("STORAGE ERROR");sessionInit(config.neighborhood);portalBegin();syncInit();}
void loop(){portalLoop();uiLoop();syncLoop();delay(2);}
