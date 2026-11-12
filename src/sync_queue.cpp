#include "sync_queue.h"
#include "lead_store.h"
#include "config.h"
extern FiberConfig config;
void syncInit(){} bool syncConfigured(){return config.backendUrl.startsWith("https://")&&config.backendToken.length()>0;}uint32_t syncPending(){return pendingCount();}void syncLoop(){/* disabled until CA provisioning and backend contract are explicit */}
