#include "display_ui.h"
#include <M5Cardputer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "lead_store.h"
#include "session.h"
#include "config.h"
extern FiberConfig config; static uint32_t last=0;
void uiBegin(){M5Cardputer.Display.setRotation(1);M5Cardputer.Display.setTextSize(1);}
void uiLoop(){if(millis()-last<1000)return;last=millis();auto &d=M5Cardputer.Display;d.fillScreen(BLACK);d.setCursor(2,2);d.setTextColor(WHITE);d.println("FIBERSCOUT V1");d.printf("WiFi: ACTIVE\nSSID: %.20s\nClients: %d\nLeads Today: %lu\nTotal Leads: %lu\nPending Sync: %lu\nStorage: %s\nStatus: %s\n\n[SPACE] session %s\n[R] refresh",config.ssid.c_str(),WiFi.softAPgetStationNum(),(unsigned long)todayCount(),(unsigned long)leadCount(),(unsigned long)pendingCount(),LittleFS.totalBytes()>LittleFS.usedBytes()?"OK":"FULL",sessionState().active?"SESSION":"READY",sessionState().active?"STOP":"START");if(M5Cardputer.Keyboard.isChange()&&M5Cardputer.Keyboard.isPressed()){auto keys=M5Cardputer.Keyboard.keysState();for(char c:keys.word)if(c==' '){sessionToggle();break;}}}
