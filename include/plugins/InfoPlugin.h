#pragma once

#ifdef ESP32
#include <HTTPClient.h>
#endif
#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#endif
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "PluginManager.h"

class InfoPlugin : public Plugin
{
private:
  struct tm timeinfo;

  int previousMinutes;
  int previousHour;
  std::vector<int> time;

  static const int clockPosX = -1;
  static const int clockPosY = 11;

  unsigned long weatherLastUpdate = 0;
  int weatherRefreshRate = (1000 * 60 * 30);
  HTTPClient http;

  std::vector<int> degrees = {0};
  bool negativeTemp = false;

  void drawFirst(bool, bool);
  bool updateTime();
  bool updateWeather();

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
