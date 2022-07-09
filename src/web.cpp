#include "web.h"
#include <AsyncElegantOTA.h> // Не переносить в HEADER файл, кривая библиотека.

AsyncWebServer server(80); // Создаем объект AsyncWebServer, порт 80

const char *namePlaceHolder PROGMEM = R"=====(
<div style="font-size: 1.2rem; text-transform: uppercase; font-weight: bold;">%s</div>
)=====";

const char *timePlaceHolder PROGMEM = R"=====(
<span id="time" style="font-style: italic">%s</span>
)=====";

const char *sensorsPlaceHolder PROGMEM = R"=====(
<div style="font-style: italic; margin-bottom: 2rem">
Temperature: %s&#x000B0;C<br>
Humidity: %s&#x00025;<br>
Pressure: %smmHg (%s&#x000B0;C)
</div>
)=====";

const char *mainIndexPlaceHolder PROGMEM = R"=====(
 <input type="text" id="input_1" style="text-align: center; width: 5rem; font-size: 1rem; font-weight: bold;" placeholder="VALUE"/>
 <button type="button" id="button3" onclick="processingButton(this)" style="margin-left: 1rem">SEND</button>
 <br><br>
 <h3>PIN %s</h3>
 <label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="switch%s" %s>
 <span class="slider"></span></label><h3>Enable button</h3><label class="switch">
 <input type="checkbox" onchange="showElement(button1, this.checked)" id="switchreset" ><span class="slider">
 </span></label>
 <br><br>
 <button type="button" id="button1" disabled="false" onclick="processingButton(this)">BUTTON1</button>
)=====";

const char *mainSettingsPlaceHolder PROGMEM = R"=====(
<form action="/settingsSave" method="get">
  <div style="margin-bottom: 1rem">
    <label for="name">Name:</label><input name="name" id="name" value="%s">
    <div style="margin-top: 2.5rem;">
      <button type="submit">Save</button>
    </div>
  </div>
</form>
)=====";

void initWebServer()
{
  Serial.print(F("WEB: Server initializing "));
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", String(), false, processingPlaceHolder); });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/style.css", "text/css"); });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/script.js", "text/JavaScript"); });
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plane", asctime(&timeStructure)); });
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/settings.html", String(), false, processingPlaceHolder); });
  server.on("/settingsSave", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Config tmpConfig;
              tmpConfig.name = request->getParam("name")->value();
              Serial.printf("\"%s\" - \"%s\"", config.name.c_str(), tmpConfig.name.c_str());
              if (tmpConfig.name != "" && tmpConfig.name != config.name){
                Serial.print(" - TRUE\n");
                config.name = tmpConfig.name;
                saveConfiguration(config);
              } else
                Serial.print(" - FALSE\n");
              request->send(LittleFS, "/settings.html", String(), false, processingPlaceHolder); });
  // Тест получения данных в формате JSON
  server.on("/json", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", "{\"message\":\"Not found\"}"); });
  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String webElement = "Null";
              // GET значения /set?element=<Message1>&value=<Message2>
              if (request->hasParam("element"))
              {
                webElement = request->getParam("element")->value();
                inputValue = request->hasParam("value") ? request->getParam("value")->value().toInt() : 0;
                buttonNumber = webElement.indexOf("button") >= 0 ? webElement.substring(6).toInt() : 0;
                if (webElement.indexOf("switch") >= 0)
                  digitalWrite(webElement.substring(6).toInt(), inputValue);
              }
              Serial.println("WEB: " + webElement + " - " + inputValue);
              request->send(200, "text/plain", "OK"); });

  server.onNotFound(notFound);
  AsyncElegantOTA.begin(&server); // Запуск ElegantOTA для прошивки по OTA
  server.begin();                 // Запуск AsyncWebServer
  Serial.println(F("[OK]"));
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String processingPlaceHolder(const String &var)
{
  if (var == "NAMEPLACEHOLDER")
  {
    char buffer[strlen(namePlaceHolder) + config.name.length()];
    sprintf_P(buffer, namePlaceHolder, config.name.c_str());
    // Serial.print("strlen(namePlaceHolder): " + String(strlen(namePlaceHolder)) +
    //              "; config.name.length(): " + String(config.name.length()) +
    //              "; sizeof(buffer): " + String(sizeof(buffer)) +
    //              "; strlen(buffer): " + String(strlen(buffer)));

    return String(buffer);
  }
  if (var == "TIMEPLACEHOLDER")
  {
    char buffer[strlen(timePlaceHolder) + strlen(asctime(&timeStructure))];
    sprintf_P(buffer, timePlaceHolder, asctime(&timeStructure));
    return String(buffer);
  }
  if (var == "SENSORSPLACEHOLDER")
  {
    char buffer[strlen(sensorsPlaceHolder) + 10];
    sprintf_P(buffer, sensorsPlaceHolder, String(temperature1, 1), String(humidity), String(pressure), String(temperature2, 1));
    return String(buffer);
  }
  if (var == "MAININDEXPLACEHOLDER")
  {
    char buffer[strlen(mainIndexPlaceHolder) + 10];
    sprintf_P(buffer, mainIndexPlaceHolder, String(PIN1), String(PIN1), getPinStringStatus(PIN1));
    return String(buffer);
  }
  if (var == "MAINSETTINGSPLACEHOLDER")
  {
    char buffer[strlen(mainSettingsPlaceHolder) + 10];
    sprintf_P(buffer, mainSettingsPlaceHolder, config.name.c_str());
    return String(buffer);
  }
  return "";
}

// Проверяем текущее состояние pin и возвращаем "checked" или пустую строку.
String getPinStringStatus(uint8_t pin)
{
  if (digitalRead(pin))
    return "checked";
  else
    return "";
}
