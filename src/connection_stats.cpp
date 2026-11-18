#include "connection_stats.h"
#include <Preferences.h>
#include <time.h>
#include <sys/time.h>

namespace {
constexpr uint32_t MAGIC = 0x46534332; // FSC2
constexpr uint32_t SAVE_INTERVAL_MS = 30000;
struct Bucket { uint32_t period; uint32_t count; };
struct Persisted {
  uint32_t magic;
  uint32_t lifetime;
  uint32_t beforeClock;
  Bucket hours[24];
  Bucket days[31];
};
Persisted data = {};
Preferences prefs;
portMUX_TYPE pendingMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t pendingJoins = 0;
uint32_t lastSaveMs = 0;
bool dirty = false;
bool trustedClock = false;
uint32_t preClockThisBoot = 0;
uint32_t firstPreClockMs = 0;

uint32_t epochNow() {
  time_t t = time(nullptr);
  if (!trustedClock || t < 1700000000) return 0;
  return static_cast<uint32_t>(t);
}
uint32_t dayKey(uint32_t epoch) {
  time_t t = static_cast<time_t>(epoch);
  struct tm local{};
  localtime_r(&t, &local);
  return static_cast<uint32_t>(local.tm_year + 1900) * 1000U +
         static_cast<uint32_t>(local.tm_yday);
}
void addBucket(Bucket *values, size_t size, uint32_t key, uint32_t by) {
  Bucket &b = values[key % size];
  if (b.period != key) {
    b.period = key;
    b.count = 0;
  }
  if (UINT32_MAX - b.count < by) b.count = UINT32_MAX;
  else b.count += by;
}
void addJoins(uint32_t n) {
  if (n == 0) return;
  data.lifetime = UINT32_MAX - data.lifetime < n ? UINT32_MAX : data.lifetime + n;
  uint32_t epoch = epochNow();
  if (!epoch) {
    data.beforeClock = UINT32_MAX - data.beforeClock < n ? UINT32_MAX : data.beforeClock + n;
    if (preClockThisBoot == 0) firstPreClockMs = millis();
    preClockThisBoot = UINT32_MAX - preClockThisBoot < n ? UINT32_MAX : preClockThisBoot + n;
  } else {
    addBucket(data.hours, 24, epoch / 3600, n);
    addBucket(data.days, 31, dayKey(epoch), n);
  }
  dirty = true;
}
void saveIfDue() {
  if (!dirty || millis() - lastSaveMs < SAVE_INTERVAL_MS) return;
  if (prefs.putBytes("counters", &data, sizeof(data)) == sizeof(data)) {
    dirty = false;
  }
  lastSaveMs = millis();
}
uint32_t sumHours(uint32_t minHour, uint32_t maxHour) {
  uint32_t sum = 0;
  for (const auto &b : data.hours) {
    if (b.period >= minHour && b.period <= maxHour) sum += b.count;
  }
  return sum;
}
uint32_t sumDays(uint32_t nowEpoch, uint32_t days) {
  // Compare each stored local day to local calendar days, respecting DST.
  time_t t = static_cast<time_t>(nowEpoch);
  struct tm local{};
  localtime_r(&t, &local);
  local.tm_hour = 12; local.tm_min = 0; local.tm_sec = 0;
  uint32_t total = 0;
  for (uint32_t age = 0; age < days; ++age) {
    struct tm prior = local;
    prior.tm_mday -= static_cast<int>(age);
    time_t priorEpoch = mktime(&prior);
    uint32_t key = dayKey(static_cast<uint32_t>(priorEpoch));
    const Bucket &b = data.days[key % 31];
    if (b.period == key) total += b.count;
  }
  return total;
}
} // namespace

void connectionStatsBegin() {
  memset(&data, 0, sizeof(data));
  prefs.begin("fsc-stats", false);
  if (prefs.getBytesLength("counters") == sizeof(data)) {
    Persisted loaded{};
    if (prefs.getBytes("counters", &loaded, sizeof(loaded)) == sizeof(loaded) && loaded.magic == MAGIC)
      data = loaded;
  }
  data.magic = MAGIC;
  setenv("TZ", "CST6CDT,M3.2.0/2,M11.1.0/2", 1);
  tzset();
  lastSaveMs = millis();
}

void connectionStatsRecordJoinFromWifiEvent() {
  // The Wi-Fi event callback runs in another task. Never use Preferences or
  // allocate memory inside it.
  portENTER_CRITICAL(&pendingMux);
  if (pendingJoins != UINT32_MAX) ++pendingJoins;
  portEXIT_CRITICAL(&pendingMux);
}
void connectionStatsLoop() {
  uint32_t n = 0;
  portENTER_CRITICAL(&pendingMux);
  n = pendingJoins;
  pendingJoins = 0;
  portEXIT_CRITICAL(&pendingMux);
  addJoins(n);
  saveIfDue();
}
bool connectionStatsSetClock(uint32_t epoch) {
  // First page visit after boot supplies approximate clock time.
  // Public untrusted clock sources affect only display grouping, not security.
  if (trustedClock || epoch < 1700000000U || epoch > 2082758400U) return false;
  struct timeval tv{};
  tv.tv_sec = epoch;
  if (settimeofday(&tv, nullptr) != 0) return false;
  trustedClock = true;
  // Count only associations from the first five minutes before a portal
  // browser clock arrives. Older undated associations stay unassigned.
  if (preClockThisBoot > 0 && millis() - firstPreClockMs <= 5UL * 60000UL) {
    addBucket(data.hours, 24, epoch / 3600, preClockThisBoot);
    addBucket(data.days, 31, dayKey(epoch), preClockThisBoot);
    data.beforeClock = data.beforeClock >= preClockThisBoot ?
                       data.beforeClock - preClockThisBoot : 0;
    dirty = true;
  }
  preClockThisBoot = 0;
  return true;
}
void connectionStatsNtpSynced(uint32_t epoch) {
  if (epoch < 1700000000U || epoch > 2082758400U) return;
  // SNTP already set system time; mark the date as trusted, even if the
  // captive browser previously supplied an approximate timestamp.
  if (!trustedClock) connectionStatsSetClock(epoch);
  trustedClock = true;
}
ConnectionTotals connectionStatsGet() {
  ConnectionTotals out{};
  out.lifetime = data.lifetime;
  out.beforeClock = data.beforeClock;
  uint32_t epoch = epochNow();
  out.clockValid = epoch != 0;
  if (!epoch) return out;
  const uint32_t hour = epoch / 3600;
  out.last24Hours = sumHours(hour >= 23 ? hour - 23 : 0, hour);
  out.today = sumDays(epoch, 1);
  out.last7Days = sumDays(epoch, 7);
  out.last30Days = sumDays(epoch, 30);
  return out;
}
