#include "timentp.h"

struct tm timeStructure;
uint16_t timeoutGetNtp = 1440; // Таймер периода обновления времени по NTP (в минутах).
time_t lastNtpTime = 0;        // Время последней синхронизации по NTP.
uint32_t timerNtpTry = 0;      // Таймер в миллис с последней попытки соединения с сервером NTP.
uint32_t timerNtpSuccess = 0;  // Выремя миллис с последнего успешного получения времени с сервера.
boolean flagRTC = false;
RTC_DS1307 rtc;

/**
 * Инициализация аппаратного модуля времени.
 */
void initRTC()
{
    if (!rtc.begin())
    {
        flagRTC = false;
        Serial.println(F("TIME: Couldn't find I2C RTC module!"));
    }
    else
    {
        flagRTC = true;
        Serial.printf("TIME: RTC time: %s\n", rtc.now().timestamp().c_str());
    }
}

/**
 * Обновляем время с запросом NTP с интервалом TIMER_GET_NTP.
 * В случае отсутствия связи времнное обновление вреени по millis().
 * Если присутствует модуль RTC работаем с ним.
 * @return UNIX time или false
 */
time_t refreshTime()
{
    time_t timeUnix = 0;
    if (!timerNtpTry || (millis() - timerNtpTry) / 60000 >= timeoutGetNtp) // Условие для запроса времени на сервере NTP
    {
        timerNtpTry = millis();
        time_t nowNTP = getNtpTime();
        if (nowNTP)
        {
            timeUnix = lastNtpTime = nowNTP;
            timerNtpSuccess = millis();
            timeoutGetNtp = 1440; // Следующее обновление времени по NTP через 24 часа
            if (flagRTC)
            {
                localtime_r(&timeUnix, &timeStructure);
                Serial.printf("TIME: Set NTP to RTC: %s", rtc.now().timestamp().c_str()); // Заполняем структуру времени.
                rtc.adjust(DateTime(timeUnix));
                Serial.printf(" -> %s\n", rtc.now().timestamp().c_str());
                return timeUnix;
            }
        }
        else
            timeoutGetNtp = 15; // Повторная попытка соединиться с NTP через 15 минут
    }

    if (flagRTC)
        timeUnix = rtc.now().unixtime();
    else
        timeUnix = lastNtpTime + (millis() - timerNtpSuccess) / 1000; // Прибавляем к времени полученному с NTP время с запуска модуля и вычитаем время с момента получения данных с NTP.
    localtime_r(&timeUnix, &timeStructure);                           // Заполняем структуру времени.
    return timeUnix;                                                  // Возвращаем время в формате UNIX TIME
}

/**
 * Запрос времени с сервера pool.ntp.org.
 * @return UNIX time или false
 */
time_t getNtpTime()
{
#define NTP_PACKET_SIZE 48 // NTP время содержится в первых 48 байтах сообщения.
#define LOCAL_PORT 2390    // Локальный порт для получения UDP пакетов
    time_t timeUnix = 0;
    WiFiUDP udp;                           // Экземпляр UDP, позволяющий отправлять и получать пакеты через UDP.
    IPAddress timeServerIP;                // IP адрес NTP сервера.
    uint8_t packetBuffer[NTP_PACKET_SIZE]; // Буфер для хранения пакетов.
    const char *ntpServerName = "pool.ntp.org";
    WiFi.hostByName(ntpServerName, timeServerIP); // Получаем IP адрес по имени домена.
    Serial.printf("TIME: Connecting to %s:%d", timeServerIP.toString().c_str(), LOCAL_PORT);
    udp.begin(LOCAL_PORT);

    for (uint8_t i = 1; i <= 3; i++)
    {
        memset(packetBuffer, 0, NTP_PACKET_SIZE); // Устанавливаем все буферы равными 0.
        // Параметры инициализации необходимы для формирования пакета запроса времени от сервера.
        packetBuffer[0] = 0b11100011; // LI, Version, Mode
        packetBuffer[1] = 0;          // Stratum, or type of clock
        packetBuffer[2] = 6;          // Polling Interval
        packetBuffer[3] = 0xEC;       // Peer Clock Precision
        // 8 нулевых байтов для Root Delay & Root Dispersion
        packetBuffer[12] = 49;
        packetBuffer[13] = 0x4E;
        packetBuffer[14] = 49;
        packetBuffer[15] = 52;

        udp.beginPacket(timeServerIP, 123); // NTP запрос на порт 123.
        udp.write(packetBuffer, NTP_PACKET_SIZE);
        udp.endPacket();

        delay(1000); // Ожидание получения ответа.

        int cb = udp.parsePacket();
        if (!cb)
        {
            i != 3 ? Serial.print(".") : Serial.print(". [FAIL]\n");
        }
        else
        {
            udp.read(packetBuffer, NTP_PACKET_SIZE);                      // Получили пакет, считываем пакет в буффер.
            uint32_t highWord = word(packetBuffer[40], packetBuffer[41]); // Время содержится начиная с 40 байта пришедшего пакета,
            uint32_t lowWord = word(packetBuffer[42], packetBuffer[43]);  // и занимает 4 байта.
            uint32_t secsSince1900 = highWord << 16 | lowWord;            // Совмещаем 4 байта (two words) into a long integer
            // secsSince1900 время NTP (сколько прошло секунд считая с 1 Января 1900)
            timeUnix = secsSince1900 - 2208988800UL + 10800UL; // Конвертируем время из NTP в Unix Time который начинает отсчет с 1 Января 1970 и вычитаем 70 лет, в секундах (2208988800) и прибаляем 3 часа к Гринвичу (10800).
            Serial.print(" [OK] Recieve " + String(cb) + " bytes, epoch = " + String(secsSince1900) + ", timeUnix = " + String(timeUnix) + "\n");
            break;
        }
    }
    return timeUnix;
}

/**
 * Получить время работы модуля без перезагрузки.
 * @return Секунды (time_t).
 */
uint32_t getOnlineTime()
{
    static uint32_t onlineTimeSec = 0;
    static uint32_t lastMillis = 0;
    uint32_t nowMillis = millis();
    onlineTimeSec += lastMillis < nowMillis ? (nowMillis - lastMillis) / 1000 : (4294967295 - lastMillis + nowMillis) / 1000;
    lastMillis = nowMillis;
    return onlineTimeSec;
}
