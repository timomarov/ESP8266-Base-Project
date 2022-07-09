#include "config.h"
// **************************************************
// ****************** CONFIG BLOCK ******************
// **************************************************
#define CONFIG_FILE "/config.json"
Config config;

/**
 * Загрузка конфигурационного файла (LittleFS) в структуру Config
 * @param config ссылка на структуру с параметрами
 * @return true или false
 */
bool loadConfiguration(Config &config)
{
    // Открываем файл для чтения
    Serial.println(F("CFG: Loading configuration..."));
    File configFile = LittleFS.open(CONFIG_FILE, "r");
    if (!configFile)
    {
        Serial.println("CFG: Failed to open config file");
        return false;
    }

    // Документация Arduino JSON: https://arduinojson.org/v6/
    // Расчет необходимой памяти под JSON массив https://arduinojson.org/v6/assistant/
    DynamicJsonDocument jsonDoc(512);
    DeserializationError error = deserializeJson(jsonDoc, configFile);
    if (error)
    {
        Serial.print(F("JSON: deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }
    // Визуализация распознанного JSON (для отладки)
    Serial.print(F("CFG: Loading from file "));
    // serializeJsonPretty(jsonDoc, Serial);
    config.name = jsonDoc["name"].as<String>();
    config.ssid = jsonDoc["ssid"].as<String>();
    config.ssidPwd = jsonDoc["ssidPwd"].as<String>();
    config.host = jsonDoc["host"].as<String>();
    config.port = jsonDoc["port"].as<uint16_t>();
    config.botToken = jsonDoc["botToken"].as<String>();
    config.adminChatId = jsonDoc["adminChatId"].as<int64_t>();
    config.sqlIp = jsonDoc["sqlIp"].as<String>();
    config.sqlPort = jsonDoc["sqlPort"].as<uint16_t>();
    config.sqlUser = jsonDoc["sqlUser"].as<String>();
    config.sqlPwd = jsonDoc["sqlPwd"].as<String>();
    config.sqlDb = jsonDoc["sqlDb"].as<String>();

    Serial.println();

    configFile.close();
    return true;
}

/**
 * Загрузка конфигурационного файла (LittleFS) в структуру Config
 * @param config ссылка на структуру с параметрами
 */
// Запись конфигурационного файла (LittleFS)
void saveConfiguration(const Config &config)
{
    // Open file for writing
    File configFile = LittleFS.open(CONFIG_FILE, "w+");
    if (!configFile)
    {
        Serial.println(F("CFG: Failed to create file"));
        return;
    }

    // Документация Arduino JSON: https://arduinojson.org/v6/
    // Расчет необходимой памяти под JSON массив https://arduinojson.org/v6/assistant/
    DynamicJsonDocument jsonDoc(512);
    jsonDoc["name"] = config.name;
    jsonDoc["ssid"] = config.ssid;
    jsonDoc["ssidPwd"] = config.ssidPwd;
    jsonDoc["host"] = config.host;
    jsonDoc["port"] = config.port;
    jsonDoc["botToken"] = config.botToken;
    jsonDoc["adminChatId"] = config.adminChatId;
    jsonDoc["sqlIP"] = config.sqlIp;
    jsonDoc["sqlPort"] = config.sqlPort;
    jsonDoc["sqlUser"] = config.sqlUser;
    jsonDoc["sqlPwd"] = config.sqlPwd;
    jsonDoc["sqlDb"] = config.sqlDb;

    // Serialize JSON to file
    if (serializeJson(jsonDoc, configFile) == 0)
    {
        Serial.println(F("CFG: Failed to write to file"));
    }
    // Close the file (File's destructor doesn't close the file)
    configFile.close();
}

// Вывести содержимое файла в Serial порт
void printFile(String filename)
{
    // Open file for reading
    File file = LittleFS.open(filename, "r");
    if (!file)
    {
        Serial.print(F("CFG: Failed to read file config file"));
        return;
    }
    Serial.print("CFG: ");
    // Extract each characters by one by one
    while (file.available())
    {
        Serial.print((char)file.read());
    }
    Serial.println();

    // Close the file (File's destructor doesn't close the file)
    file.close();
}