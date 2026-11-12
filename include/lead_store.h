#pragma once
#include <Arduino.h>
constexpr size_t MAX_FIRST=40,MAX_LAST=40,MAX_CONTACT=120,MAX_ADDRESS=120;
struct Lead { uint32_t sequence=0,session=0,createdMs=0; String first,last,contact,address,provider,bill,preferred,interested; bool consent=false; };
bool initLeadStore(); bool validateLead(const Lead&,String&); bool isDuplicateLead(const Lead&); bool appendLead(Lead&,String&); uint32_t leadCount(); uint32_t pendingCount(); uint32_t todayCount(); bool eraseAllLeads(); bool recoverLeadStore();
