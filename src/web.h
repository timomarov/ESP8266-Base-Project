#ifndef web_h
#define web_h

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "timentp.h"
#include "config.h"
#include "sensors.h"

// main.h
extern uint8_t buttonNumber;
extern uint16_t inputValue;
extern const uint8_t PIN1;
extern const uint8_t PIN2;
// config.h
extern struct Config config;
// timentp.h
extern struct tm timeStructure;
// sensors.h
extern float temperature1;
extern float temperature2;
extern uint16_t humidity;
extern uint16_t pressure;

void initWebServer();
void notFound(AsyncWebServerRequest *request);
String processingPlaceHolder(const String &var);
String getPinStringStatus(uint8_t pin);

#endif