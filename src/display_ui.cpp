#include "display_ui.h"
#include <M5Cardputer.h>
#include <WiFi.h>
#include "session.h"
#include "config.h"

extern FiberConfig config;
static uint32_t last = 0;

void uiBegin() {
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextSize(1);
}
void uiLoop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    auto keys = M5Cardputer.Keyboard.keysState();
    for (char c : keys.word) {
      if (c == ' ') { sessionToggle(); break; }
    }
  }
  if (millis() - last < 1000) return;
  last = millis();
  auto &d = M5Cardputer.Display;
  d.fillScreen(BLACK);
  d.setCursor(3, 3);
  d.setTextColor(0xF81F);
  d.println("FIBERSCOUT V2");
  d.setTextColor(WHITE);
  d.printf("WiFi: ACTIVE\nSSID: %.22s\nClients: %d\nPortal: 192.168.4.1\nPlans: 4 (offline)\nContact: SMS / call / email\nUplink: NONE\nStatus: %s\n\n[SPACE] session %s",
      config.ssid.c_str(),
      WiFi.softAPgetStationNum(),
      sessionState().active ? "SESSION" : "READY",
      sessionState().active ? "STOP" : "START");
}
