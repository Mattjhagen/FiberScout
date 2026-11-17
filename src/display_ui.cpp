#include "display_ui.h"
#include <M5Cardputer.h>
#include <WiFi.h>
#include "session.h"
#include "config.h"
#include "web_portal.h"

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
  d.println("FIBERSCOUT CAR");
  d.setTextColor(WHITE);
  d.printf("WiFi: ACTIVE | %d clients\\nSSID: %.22s\\nPortal: 192.168.4.1\\nTX: %d dBm | AP resets: %lu\\nUptime: %luh %lum\\nHeap: %lu KB\\nUplink: OFFLINE\\nStatus: %s\\n[SPACE] session %s",
      WiFi.softAPgetStationNum(),
      config.ssid.c_str(),
      static_cast<int>(portalTxPowerDbm()),
      static_cast<unsigned long>(portalWifiRestarts()),
      static_cast<unsigned long>(portalUptimeSeconds() / 3600),
      static_cast<unsigned long>((portalUptimeSeconds() / 60) % 60),
      static_cast<unsigned long>(ESP.getFreeHeap() / 1024),
      sessionState().active ? "SESSION" : "READY",
      sessionState().active ? "STOP" : "START");
}
