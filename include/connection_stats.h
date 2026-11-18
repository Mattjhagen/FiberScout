#pragma once
#include <Arduino.h>

// Counts Wi-Fi station associations, not unique people or phones.
// No MAC addresses, IP addresses, or other identifiers are retained.
struct ConnectionTotals {
  uint32_t today;
  uint32_t last24Hours;
  uint32_t last7Days;
  uint32_t last30Days;
  uint32_t lifetime;
  bool clockValid;
  uint32_t beforeClock;
};
void connectionStatsBegin();
void connectionStatsLoop();
void connectionStatsRecordJoinFromWifiEvent();
bool connectionStatsSetClock(uint32_t unixSeconds);
void connectionStatsNtpSynced(uint32_t unixSeconds);
ConnectionTotals connectionStatsGet();
