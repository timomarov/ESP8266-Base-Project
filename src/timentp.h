#ifndef timentp_h
#define timentp_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "RTClib.h"

void initRTC();
time_t refreshTime();
time_t getNtpTime();
uint32_t getOnlineTime();

#endif