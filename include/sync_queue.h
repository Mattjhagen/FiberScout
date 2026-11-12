#pragma once
#include <Arduino.h>
void syncInit(); bool syncConfigured(); uint32_t syncPending(); void syncLoop();
