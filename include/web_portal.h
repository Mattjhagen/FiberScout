#pragma once
#include <Arduino.h>
void portalBegin();
void portalLoop();
uint32_t portalWifiRestarts();
int8_t portalTxPowerDbm();
uint32_t portalUptimeSeconds();
