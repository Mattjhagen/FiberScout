#pragma once
#include <Arduino.h>
struct FiberConfig { String ssid="Try T-Mobile Fiber"; String collector="FiberScout operator"; String privacyNotice="Your information is collected by the FiberScout operator only to respond to this request. Request deletion from the operator."; String neighborhood="Unassigned"; String backendUrl; String backendToken; };
bool loadConfig(FiberConfig &cfg); bool saveConfig(const FiberConfig &cfg);
