#ifndef telegram_h
#define telegram_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <AsyncTelegram2.h>
#include "config.h"
#include "timentp.h"
#include "version.h"

// main.h
extern String mac;
// config.h
extern struct Config config;
// timentp.h
extern struct tm timeStructure;
// sensors.h
extern float temperature1;
extern float temperature2;
extern uint16_t humidity;
extern uint16_t pressure;
extern uint16_t analog;

void initTelegram();
void sendTelegramMessage(String message);
void getTelegramUpdate();

#endif