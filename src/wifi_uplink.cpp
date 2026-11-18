#include "wifi_uplink.h"
#include "connection_stats.h"
#include <WiFi.h>
#include <Preferences.h>
#include <time.h>
#include <sntp.h>

namespace {
String ssid;
String password;
bool ntpStarted = false;
bool ntpConfirmed = false;
volatile bool ntpPending = false;
uint32_t lastAttemptMs = 0;
const uint32_t RETRY_MS = 30000;

void ntpCallback(struct timeval *) {
  ntpPending = true;
}
void connectSaved() {
  if (ssid.isEmpty()) return;
  if (WiFi.getMode() != WIFI_AP_STA) WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  lastAttemptMs = millis();
  Serial.println("FiberScout uplink: connecting to saved hotspot");
}
}

void uplinkBegin() {
  Preferences p;
  p.begin("fsc-uplink", true);
  ssid = p.getString("ssid", "");
  password = p.getString("pass", "");
  p.end();
  sntp_set_time_sync_notification_cb(ntpCallback);
  if (!ssid.isEmpty()) connectSaved();
}
void uplinkLoop() {
  if (ntpPending) {
    ntpPending = false;
    time_t now = time(nullptr);
    if (now > 1700000000) {
      connectionStatsNtpSynced(static_cast<uint32_t>(now));
      ntpConfirmed = true;
      Serial.println("FiberScout time: NTP synchronized");
    }
  }
  if (ssid.isEmpty()) return;
  if (WiFi.status() == WL_CONNECTED) {
    if (!ntpStarted) {
      configTime(0, 0, "time.google.com", "pool.ntp.org", "time.cloudflare.com");
      // The stats module sets America/Chicago; configTime resets TZ to UTC.
      setenv("TZ", "CST6CDT,M3.2.0/2,M11.1.0/2", 1);
      tzset();
      ntpStarted = true;
      Serial.println("FiberScout uplink: internet connected; requesting NTP");
    }
    return;
  }
  ntpStarted = false;
  if (millis() - lastAttemptMs >= RETRY_MS) {
    WiFi.disconnect(false, false);
    connectSaved();
  }
}
bool uplinkConfigured() { return !ssid.isEmpty(); }
bool uplinkConnected() { return WiFi.status() == WL_CONNECTED; }
bool uplinkNtpSynced() { return ntpConfirmed; }
String uplinkSsid() { return ssid; }
String uplinkIp() { return uplinkConnected() ? WiFi.localIP().toString() : String("--"); }
bool uplinkSaveCredentials(const String &newSsid, const String &newPass) {
  if (newSsid.length() < 1 || newSsid.length() > 32 || newPass.length() > 63) return false;
  Preferences p;
  if (!p.begin("fsc-uplink", false)) return false;
  bool ok = p.putString("ssid", newSsid) && p.putString("pass", newPass);
  p.end();
  if (!ok) return false;
  ssid = newSsid;
  password = newPass;
  ntpStarted = false;
  ntpConfirmed = false;
  connectSaved();
  return true;
}
void uplinkDisconnectAndForget() {
  Preferences p;
  if (p.begin("fsc-uplink", false)) { p.clear(); p.end(); }
  ssid = ""; password = "";
  ntpStarted = false;
  ntpConfirmed = false;
  WiFi.disconnect(false, false);
  WiFi.mode(WIFI_AP); // Customers retain AP, no automatic AP teardown.
}
