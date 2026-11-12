#pragma once
#include <Arduino.h>
struct SessionState { uint32_t id=0,startedMs=0,endedMs=0,submissions=0; String name="Default session",neighborhood="Unassigned"; bool active=false; };
void sessionInit(const String&); bool sessionToggle(); const SessionState& sessionState(); uint32_t activeSessionId();
