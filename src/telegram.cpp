#include "telegram.h"

#define USE_CLIENTSSL true
// Timezone definition
#include <time.h>
#define MYTZ "CET-1CEST,M3.5.0,M10.5.0/3"

#ifdef ESP8266
#include <ESP8266WiFi.h>
BearSSL::WiFiClientSecure clientTg;
BearSSL::Session sessionTg;
BearSSL::X509List certificateTg(telegram_cert);

#elif defined(ESP32)
#include <WiFi.h>
#include <WiFiClient.h>
#if USE_CLIENTSSL
#include <SSLClient.h>
#include "tg_certificate.h"
WiFiClient base_client;
SSLClient client(base_client, TAs, (size_t)TAs_NUM, A0, 1, SSLClient::SSL_ERROR);
#else
#include <WiFiClientSecure.h>
WiFiClientSecure client;
#endif
#endif

AsyncTelegram2 myBot(clientTg);

void initTelegram()
{
#ifdef ESP8266
    // Sync time with NTP, to check properly Telegram certificate
    configTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
    // Set certficate, session and some other base client properies
    clientTg.setSession(&sessionTg);
    clientTg.setTrustAnchors(&certificateTg);
    clientTg.setBufferSizes(1024, 1024);
#elif defined(ESP32)
    // Sync time with NTP
    configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
#if USE_CLIENTSSL == false
    clientTg.setCACert(telegram_cert);
#endif
#endif

    Serial.print(F("TGM: Test Telegram connection... "));
    // Set the Telegram bot properies
    myBot.setUpdateTime(2000);
    myBot.setTelegramToken(config.botToken.c_str());
    myBot.begin() ? Serial.println("[OK]") : Serial.println("[FAIL]");
    sendTelegramMessage("Запуск модуля.");
}

void sendTelegramMessage(String message)
{
    myBot.sendTo(config.adminChatId, message + "\n" +
                                         String(asctime(&timeStructure)) + "ID: " + mac + " (" + config.name + ")" +
                                         "\nSSID: " + String(config.ssid) + " IP: " + WiFi.localIP().toString() +
                                         "\nv." + VERSION + " (" + BUILD_TIMESTAMP + ")");
}

void getTelegramUpdate()
{
    // (UniversalTelegramBot) Возникают постоянные перезапуски ESP8266, переключился на Asynctelegram
    TBMessage msg;
    if (myBot.getNewMessage(msg))
    {
        String message = msg.text;
        Serial.print("TGM: Telegram Chat ID - " + String(msg.chatId) + " (" + String(msg.contact.firstName) + "). New message: " + String(message) + "\n");
        if (message == "/help")
        {
            time_t timeDiff = getOnlineTime();
            uint16_t hour = timeDiff / 3600;
            uint8_t min = (timeDiff / 60) % 60;
            uint8_t sec = timeDiff % 60;
            String strTimeDiff = String(hour) + " часов " + String(min) + " минут " + String(sec) + " секунд.";
            sendTelegramMessage("\nОнлайн: " + strTimeDiff);
        }
        else if (message == "/sensors")
        {
            sendTelegramMessage("Температура: " + String(temperature1, 1) +
                                " °C\nВлажность: " + String(humidity) +
                                " % \nДавление: " + String(pressure) + " ммРтСт" +
                                " \nАналоговый вход: " + String(analog) + " ммРтСт" +
                                " (" + analog + "°C)");
        }
        else
        {
            sendTelegramMessage("Получено сообщение: " + message);
        }
    }
}