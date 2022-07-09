#ifndef sql_h
#define sql_h

#include <Arduino.h>
#include <IPAddress.h>
#include "config.h"

extern struct Config config;

void sendSqlQuery(String sql);

#endif