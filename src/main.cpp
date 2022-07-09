#include "main.h"
#include "version.h"
// Test UX32
const uint8_t PIN1 = D4;
const uint8_t PIN2 = D5;
float temperature1 = 0;
float temperature2 = 0;
uint16_t humidity = 0;
uint16_t pressure = 0;
uint16_t analog = 0;
uint8_t buttonNumber = 0;
uint16_t inputValue = 0;
String mac = WiFi.macAddress();

// **************************************************
// ********************* SETUP **********************
// **************************************************
void setup()
{
  Serial.begin(115200);
#ifndef ESP8266
  while (!Serial)
    ;
#endif
  pinMode(PIN1, OUTPUT);
  pinMode(PIN2, OUTPUT);
  boolean ledStatus = false;
  Serial.printf("\nProject version v.%s, built %s\n", VERSION, BUILD_TIMESTAMP);

  initSensors();
  initRTC();

  // Инициализация файловой системы и загрузка конфигурации из файла
  if (LittleFS.begin())
  {
    loadConfiguration(config);
  }
  else
  {
    Serial.println(F("FS: An Error has occurred while mounting LittleFS."));
  }

  Serial.print(F("WIFI: Connecting to Wi-Fi ("));
  Serial.print(config.ssid);
  Serial.print(F(") ."));
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.ssidPwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    ledStatus = !ledStatus;
    digitalWrite(PIN1, ledStatus);
    Serial.print(".");
    delay(1000);
  }
  digitalWrite(PIN1, LOW); // Встроенный светодиод инвертен, горит при LOW.
  Serial.println("\nWIFI: SSID: " + String(config.ssid) + " IP: " + WiFi.localIP().toString());

  refreshTime();   // Первое обновление врени с NTP
  initTelegram();  // Инициализация Telegram модуля
  initWebServer(); // Инициализация WEB сервера
}

void loop()
{
  static time_t timer1 = 0;
  static time_t timer2 = 0;
  static time_t timer3 = 0;
  // ************** Таймеры ***************
  if (millis() - timer1 > 1000 || !timer1) // Таймер срабатывает раз в секунду и в момент включения
  {
    timer1 = millis();
    analog = map(analogRead(A0), 10, 1024, 0, 1024);
    refreshTime();
    getTelegramUpdate();
  }
  if (millis() - timer2 > 1000 * 60 || !timer2) // Таймер срабатывает раз в минуту и в момент включения
  {
    timer2 = millis();
    getDHT22Data(temperature1, humidity);
    getPressure(pressure, temperature2);
    sendSqlQuery(String("INSERT IGNORE INTO ") + config.sqlDb + ".main" +
                 " (mac, pingpong) VALUES ('" + mac + "', CURTIME()) ON DUPLICATE KEY UPDATE pingpong = CURTIME()");
  }
  if (millis() - timer3 > 1000 * 60 * 15 || !timer3) // Таймер срабатывает раз в секунду и в момент включения
  {
    timer3 = millis();
    sendSqlQuery(String("INSERT INTO ") + config.sqlDb + ".sensors" +
                 " (mac,temperature1,temperature2,humidity,pressure,A0) VALUES ('" +
                 mac + "'," + temperature1 + "," + temperature2 + "," + humidity + "," + pressure + "," + analog + ")");
  }
  // *** Обработка нажатия на кнопки на WEB странице ***
  if (buttonNumber)
  {
    switch (buttonNumber)
    {
    case 1: // id = "button1" (index.html BUTTON1)
      break;
    case 2: // id = "button2" (settings.html SAVE)
      sendTelegramMessage("Сохранение настроек.");
      break;
    case 3: // id = "button3" (index.html SEND)
      break;
    default:
      break;
    }
    buttonNumber = 0;
  }
}
