#include "plugins/InfoPlugin.h"

#ifdef ESP8266
WiFiClient wiFiClient;
Asy;
#endif

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
          Serial.printf("[WSc] Disconnected!\n");
          break;
        case WStype_CONNECTED:
          {
            Serial.printf("[WSc] Connected to url: %s\n",  payload);

            // send message to server when Connected
				    webSocket.sendTXT("Connected");
          }
          break;
        case WStype_TEXT:
          Serial.printf("[WSc] get text: %s\n", payload);

          // send message to server
          // webSocket.sendTXT("message here");
          break;
        case WStype_BIN:
          Serial.printf("[WSc] get binary length: %u\n", length);
          //hexdump(payload, length);

          // send data to server
          // webSocket.sendBIN(payload, length);
          break;
		case WStype_ERROR:
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }

}

void InfoPlugin::setup()
{
   // loading screen
  Screen.setPixel(4, 7, 1);
  Screen.setPixel(5, 7, 1);
  Screen.setPixel(7, 7, 1);
  Screen.setPixel(8, 7, 1);
  Screen.setPixel(10, 7, 1);
  Screen.setPixel(11, 7, 1);

  previousHour = -1;
  previousMinutes = -1;

  webSocket.beginSSL("websocket-api.tibber.com", 80, "/v1-beta/gql/subscriptions");
  webSocket.onEvent(webSocketEvent);
}

void InfoPlugin::loop() {
  bool timeUpdated = updateTime();
  bool weatherUpdated = updateWeather();

  drawFirst(timeUpdated, weatherUpdated);
}

void InfoPlugin::drawFirst(bool timeUpdated, bool weatherUpdated) {
  // Draw clock on screen
  if (timeUpdated || weatherUpdated){
    Serial.printf("clearing screen: time - %d  weather - %d\n", timeUpdated, weatherUpdated);
    Screen.clear();
  }

  // Draw clock
  Screen.drawNumbers(clockPosX, clockPosY, time, Screen.getCurrentBrightness());

  int temparatureX = -1;
  // Draw temp
  if(negativeTemp) {
    Screen.drawCharacter(temparatureX, 0, Screen.readBytes(minusSymbol), 4);
    temparatureX += 4;
  }
  Screen.drawNumbers(temparatureX, 0, degrees, Screen.getCurrentBrightness());
  Screen.drawCharacter(temparatureX + (4*degrees.size()), 0, Screen.readBytes(degreeSymbol), 4, Screen.getCurrentBrightness());
}

bool InfoPlugin::updateTime() {
  // Get current time
  bool timeFetched = getLocalTime(&timeinfo);
  if(!timeFetched){
    return false;
  }

  // If time has changed
  if (previousHour == timeinfo.tm_hour && previousMinutes == timeinfo.tm_min){
    return false;
  }

    // Update previous values
  previousHour = timeinfo.tm_hour;
  previousMinutes = timeinfo.tm_min;

  // Turn hour and minutes into vectors with leading zeroes
  time = {(timeinfo.tm_hour - timeinfo.tm_hour % 10) / 10, timeinfo.tm_hour % 10,
          (timeinfo.tm_min - timeinfo.tm_min % 10) / 10, timeinfo.tm_min % 10};


  // Clear vector
  return true;
}

bool InfoPlugin::updateWeather() {
    unsigned long currentTime = millis();
    unsigned long updateAt = (weatherLastUpdate == 0) ? 0 : weatherLastUpdate + weatherRefreshRate;
    if (currentTime <= updateAt) {
      //Serial.printf("Timeout not triggered - currentTime - %d updatedAt -  %d\n", currentTime, updateAt);
      return false;
    }

    String weatherApiString = "https://wttr.in/" + String(WEATHER_LOCATION) + "?format=j2&lang=en";
    bool begin = false;
#ifdef ESP32
    begin = http.begin(weatherApiString);
#endif
#ifdef ESP8266
    being = http.begin(wiFiClient, weatherApiString);
#endif

    if(!begin){
      Serial.printf("Could not begin to fetch weather, got status code %d\n", begin);
      return false;
    }

    int code = http.GET();
    if (code != HTTP_CODE_OK) {
      Serial.printf("Could not fetch weather, got status code %d\n", code);
      return false;
    }

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, http.getString());

    int temperature = round(doc["current_condition"][0]["temp_C"].as<float>());

    negativeTemp = temperature < 0;

    if (temperature >= 10){
      degrees = {(temperature - temperature % 10) / 10, temperature % 10};
    }
    else if (temperature <= -10)
    {
      temperature *= -1;
      degrees = {(temperature - temperature % 10) / 10, temperature % 10};
    }
    else if (temperature >= 0)
    {
      degrees = {temperature};
    }
    else
    {
      degrees = {-temperature};
    }

    weatherLastUpdate = millis();
    return true;
}

const char *InfoPlugin::getName() const {
  return "Info";
}
