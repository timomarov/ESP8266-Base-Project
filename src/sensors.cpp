#include "sensors.h"

DHT dht22(D4, DHT22);
Adafruit_BMP085 bmp180;

void initSensors()
{
    dht22.begin();
    if (!bmp180.begin())
    {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        while (1)
        {
        }
    }
}

void getDHT22Data(float &temperature, uint16_t &humidity)
{
    temperature = (float)((int)(dht22.readTemperature() * 10)) / 10;
    humidity = dht22.readHumidity();
    if (isnan(temperature) || isnan(humidity))
        Serial.println(F("Failed to read from DHT sensor!"));
}

void getPressure(uint16_t &pressure, float &temperature)
{
    pressure = bmp180.readPressure() * 0.0075006156;
    temperature = bmp180.readTemperature();
}

void sendSensorData()
{
}