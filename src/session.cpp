#include "session.h"
#include <Preferences.h>
static SessionState s; static Preferences p;
void sessionInit(const String& n){p.begin("session",false);s.id=p.getUInt("id",0)+1;s.name=p.getString("name","Default session");s.neighborhood=n;s.active=false;s.submissions=0;p.putUInt("id",s.id);p.end();}
bool sessionToggle(){s.active=!s.active;if(s.active){s.startedMs=millis();s.endedMs=0;}else{s.endedMs=millis();}return s.active;}
const SessionState& sessionState(){return s;} uint32_t activeSessionId(){return s.active?s.id:0;}
