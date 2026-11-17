#include "web_portal.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"
#include "portal_assets.h"

extern FiberConfig config;
static WebServer server(80);
static DNSServer dns;

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
  WiFi.softAP(config.ssid.c_str());
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
}
