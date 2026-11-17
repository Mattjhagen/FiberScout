#include <Arduino.h>
#include <WiFi.h>
#include <M5Cardputer.h>
#include "config.h"
#include "session.h"
#include "web_portal.h"
#include "display_ui.h"

// V2 deliberately does not initialize the V1 personal-data store or sync queue.
// Existing V1 flash data is not actively used by this offline information portal.
FiberConfig config;
void setup() {
  Serial.begin(115200);
  delay(250);
  loadConfig(config);
  M5Cardputer.begin();
  uiBegin();
  sessionInit(config.neighborhood);
  portalBegin();
  Serial.println("FiberScout V2 offline portal started at http://192.168.4.1/");
}
void loop() {
  portalLoop();
  uiLoop();
  delay(2);
}
