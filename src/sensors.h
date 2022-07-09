#ifndef sensors_h
#define sensors_h

#include <DHT.h>
#include <Adafruit_BMP085.h>

void initSensors();
void getDHT22Data(float &temperature, uint16_t &humidity);
void getPressure(uint16_t &pressure, float &temperature);

#endif