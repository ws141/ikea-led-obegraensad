#pragma once

#include "ESPAsyncWebServer.h"

void handleMessage(AsyncWebServerRequest *request);
void handleMessageRemove(AsyncWebServerRequest *request);
void handleGetStatus(AsyncWebServerRequest *request);
void handleGetMetadata(AsyncWebServerRequest *request);
void handleSetPlugin(AsyncWebServerRequest *request);
void handleSetBrightness(AsyncWebServerRequest *request);
void handleGetData(AsyncWebServerRequest *request);
void handleSetRotation(AsyncWebServerRequest *request);
