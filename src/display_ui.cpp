#include "display_ui.h"
#include <M5Cardputer.h>
#include <WiFi.h>
#include "session.h"
#include "config.h"
#include "web_portal.h"
#include "connection_stats.h"

extern FiberConfig config;

namespace {
constexpr uint16_t MAGENTA = 0xE00E; // T-Mobile #E20074 in RGB565
constexpr uint16_t PINK = 0xFB15;
constexpr uint16_t BLACK_BG = 0x0000;
constexpr uint16_t PANEL = 0x1082;
constexpr uint16_t PANEL_EDGE = 0x3987;
constexpr uint16_t LIGHT = 0xFFFF;
constexpr uint16_t MUTED = 0xB5B6;
constexpr uint16_t GREEN = 0x5EE9;
uint32_t lastPaint = 0;
uint8_t activeScreen = 0; // 0 = status, 1 = connection statistics

void textAt(int x, int y, const String &s, uint16_t color = LIGHT, int scale = 1) {
  auto &d = M5Cardputer.Display;
  d.setTextSize(scale);
  d.setTextColor(color);
  d.setCursor(x, y);
  d.print(s);
}

String shortSsid(const String &ssid, size_t limit = 19) {
  if (ssid.length() <= limit) return ssid;
  return ssid.substring(0, limit - 2) + "..";
}

void renderPortrait() {
  auto &d = M5Cardputer.Display;
  const int w = d.width();
  const int h = d.height();
  const int clients = WiFi.softAPgetStationNum();
  const bool apReady = (WiFi.getMode() & WIFI_MODE_AP) && WiFi.softAPIP() == IPAddress(192,168,4,1);
  const uint32_t secs = portalUptimeSeconds();

  d.fillScreen(BLACK_BG);

  // Magenta masthead with bold, readable lettering.
  d.fillRect(0, 0, w, 37, MAGENTA);
  textAt(7, 5, "FIBERSCOUT", LIGHT, 2);
  textAt(9, 26, "CAR MODE  /  V2.2", LIGHT);

  // Status strip: clearly distinguish live WiFi from failure.
  d.fillRoundRect(7, 44, w - 14, 23, 5, PANEL);
  d.fillCircle(17, 55, 4, apReady ? GREEN : MAGENTA);
  textAt(27, 51, apReady ? "HOTSPOT ONLINE" : "HOTSPOT OFFLINE");

  // Main network card. Render the SSID once rather than overflowing the panel.
  d.fillRoundRect(7, 73, w - 14, 61, 7, PANEL);
  d.drawRoundRect(7, 73, w - 14, 61, 7, PANEL_EDGE);
  textAt(15, 81, "WIFI NETWORK", PINK);
  textAt(15, 96, shortSsid(config.ssid), LIGHT);
  textAt(15, 114, "192.168.4.1", MUTED);

  // Compact diagnostics. Never print long lines that wrap into neighboring rows.
  d.fillRoundRect(7, 140, w - 14, 64, 7, PANEL);
  d.drawRoundRect(7, 140, w - 14, 64, 7, PANEL_EDGE);
  textAt(15, 147, "CLIENTS", PINK);
  textAt(w - 27, 147, String(clients), LIGHT);
  textAt(15, 160, "TX POWER", MUTED);
  textAt(75, 160, String(static_cast<int>(portalTxPowerDbm())) + " dBm");
  textAt(15, 173, "UPTIME", MUTED);
  textAt(69, 173, String(secs / 3600) + "h " + String((secs / 60) % 60) + "m");
  textAt(15, 186, "AP RESETS", MUTED);
  textAt(88, 186, String(portalWifiRestarts()));

  // Footer doubles as a reminder that customers get an offline web page.
  d.fillRect(0, h - 29, w, 29, MAGENTA);
  textAt(7, h - 24, sessionState().active ? "SESSION ACTIVE" : "READY  /  OFFLINE", LIGHT);
  textAt(7, h - 12, "FN+> STATS  SPACE", LIGHT);
}
void renderStats() {
  auto &d = M5Cardputer.Display;
  const int w = d.width();
  const int h = d.height();
  const ConnectionTotals totals = connectionStatsGet();
  d.fillScreen(BLACK_BG);

  d.fillRect(0, 0, w, 37, MAGENTA);
  textAt(7, 5, "FIBERSCOUT", LIGHT, 2);
  textAt(9, 26, "CONNECTION STATS", LIGHT);

  d.fillRoundRect(7, 43, w - 14, 23, 5, PANEL);
  textAt(14, 49, "WIFI JOINS / NOT PEOPLE", PINK);

  const char *labels[5] = { "TODAY", "LAST 24 HOURS", "LAST 7 DAYS",
                            "LAST 30 DAYS", "LIFETIME" };
  uint32_t counts[5] = { totals.today, totals.last24Hours,
                         totals.last7Days, totals.last30Days, totals.lifetime };
  for (int i = 0; i < 5; ++i) {
    int y = 70 + i * 28;
    d.fillRoundRect(7, y, w - 14, 24, 5, PANEL);
    textAt(14, y + 3, labels[i], PINK);
    if (!totals.clockValid && i < 4) {
      textAt(14, y + 14, "CLOCK NOT SET", MUTED);
    } else {
      String value = String(counts[i]);
      textAt(w - 13 - static_cast<int>(value.length()) * 6, y + 13, value, LIGHT);
    }
  }
  d.fillRect(0, h - 29, w, 29, MAGENTA);
  textAt(7, h - 24, totals.clockValid ? "CLOCK: PHONE TIME" : "CLOCK: WAITING", LIGHT);
  textAt(7, h - 12, "FN+< STATUS", LIGHT);
}

} // namespace

void uiBegin() {
  auto &d = M5Cardputer.Display;
  // The Cardputer ADV panel is 135x240; library releases differ in
  // absolute rotation conventions. Preserve the previously upright
  // orientation (1) when it is portrait, and fall back to another portrait
  // orientation if the installed M5GFX version reports landscape.
  d.setRotation(1);
  if (d.width() > d.height()) {
    d.setRotation(0);
    if (d.width() > d.height()) d.setRotation(3);
  }
  d.setTextFont(1);
  d.setTextWrap(false);
  d.setTextSize(1);
  d.fillScreen(BLACK_BG);
  if (activeScreen == 0) renderPortrait();
  else renderStats();
}

void uiLoop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    auto keys = M5Cardputer.Keyboard.keysState();
    if (keys.left || keys.right || keys.up || keys.down) {
      activeScreen = (keys.left || keys.up) ? 0 : 1;
      lastPaint = 0;
    } else if (keys.space) {
      sessionToggle();
      lastPaint = 0;
    }
  }

  // Keep the page responsive but avoid unnecessary full-frame redraws.
  const uint32_t now = millis();
  if (lastPaint != 0 && now - lastPaint < 2000) return;
  lastPaint = now;
  if (activeScreen == 0) renderPortrait();
  else renderStats();
}
