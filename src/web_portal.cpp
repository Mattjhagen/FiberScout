#include "web_portal.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"
#include "portal_assets.h"
#include <esp_wifi.h>

extern FiberConfig config;
static WebServer server(80);
static DNSServer dns;

static uint32_t apRestarts = 0;
static uint32_t apUnhealthySince = 0;
static uint32_t lastHealthCheck = 0;
static int8_t actualTxPowerDbm = -1;
static const uint32_t kHealthIntervalMs = 10000;
static const uint32_t kRecoveryDelayMs = 30000;

// ESP32 supported power configuration; no regulatory-domain override.
static void configureRadio() {
  WiFi.setSleep(false);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  int8_t quarterDbm = 0;
  if (esp_wifi_get_max_tx_power(&quarterDbm) == ESP_OK) {
    actualTxPowerDbm = quarterDbm / 4;
  }
}
static void restartAccessPoint() {
  dns.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  if (WiFi.softAP(config.ssid.c_str())) {
    configureRadio();
    dns.start(53, "*", WiFi.softAPIP());
    ++apRestarts;
    Serial.printf("FiberScout AP restored (%lu)\n", static_cast<unsigned long>(apRestarts));
  } else {
    Serial.println("FiberScout AP recovery failed; will retry");
  }
}
uint32_t portalWifiRestarts() { return apRestarts; }
int8_t portalTxPowerDbm() { return actualTxPowerDbm; }
uint32_t portalUptimeSeconds() { return millis() / 1000; }


static void serveAsset(const PortalAsset &asset) {
  Serial.printf("Portal asset: %s (%u bytes)\n", asset.path, static_cast<unsigned>(asset.size));
  server.sendHeader("Content-Encoding", "gzip");
  server.sendHeader("Cache-Control", "no-store");
  if (String(asset.path).endsWith(".vcf")) {
    server.sendHeader("Content-Disposition", "attachment; filename=\"Matthew_Hagen_TMobile_Fiber.vcf\"");
  }
  server.send_P(200, asset.mime, reinterpret_cast<const char *>(asset.data), asset.size);
}
static void captiveRedirect() {
  server.sendHeader("Cache-Control", "no-store");
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}
void portalBegin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  if (!WiFi.softAP(config.ssid.c_str())) {
    Serial.println("FiberScout AP startup failed");
  }
  configureRadio();
  dns.start(53, "*", WiFi.softAPIP());
  for (const auto &asset : portalAssets) {
    const PortalAsset *entry = &asset;
    server.on(asset.path, HTTP_GET, [entry]() { serveAsset(*entry); });
  }
  server.on("/index.html", HTTP_GET, []() { serveAsset(portalAssets[0]); });
  // Unknown hosts and OS connectivity probes must lead to the local page.
  server.onNotFound(captiveRedirect);
  server.begin();
}
void portalLoop() {
  dns.processNextRequest();
  server.handleClient();
  const uint32_t now = millis();
  if (now - lastHealthCheck < kHealthIntervalMs) return;
  lastHealthCheck = now;
  // Zero connected stations is normal, never restart for inactivity.
  const bool healthy = (WiFi.getMode() & WIFI_MODE_AP) &&
                       WiFi.softAPIP() == IPAddress(192,168,4,1);
  if (healthy) {
    apUnhealthySince = 0;
    return;
  }
  if (apUnhealthySince == 0) {
    apUnhealthySince = now;
  } else if (now - apUnhealthySince >= kRecoveryDelayMs) {
    apUnhealthySince = now;
    restartAccessPoint();
  }
}
