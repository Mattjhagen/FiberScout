#pragma once
#include <Arduino.h>
void uplinkBegin();
void uplinkLoop();
bool uplinkConfigured();
bool uplinkConnected();
bool uplinkNtpSynced();
String uplinkSsid();
String uplinkIp();
bool uplinkSaveCredentials(const String &ssid, const String &password);
void uplinkDisconnectAndForget();
