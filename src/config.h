#ifndef config_h
#define config_h

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

struct Config
{
    String name;
    String ssid;
    String ssidPwd;
    String host;
    uint16_t port;
    String botToken;
    int64_t adminChatId;
    String sqlIp;
    uint16_t sqlPort;
    String sqlUser;
    String sqlPwd;
    String sqlDb;
};

bool loadConfiguration(Config &config);
void saveConfiguration(const Config &config);
void printFile(String filename);

#endif