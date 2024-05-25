#include "utils.h"

#include "plugins/BigClockPlugin.h"

void BigClockPlugin::setup()
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
}

void BigClockPlugin::loop()
{
  // Get current time
  bool timeFetched = getLocalTime(&timeinfo);
  if(!timeFetched){
    return;
  }

  // If first fetch then clear screen
  if(previousMinutes == -1 && previousHour == -1){
    Screen.clear();
  }

  // If time has changed
  if (previousHour == timeinfo.tm_hour && previousMinutes == timeinfo.tm_min){
    return;
  }

  // Turn hour and minutes into vectors with leading zeroes
  std::vector<int> hh = {(timeinfo.tm_hour - timeinfo.tm_hour % 10) / 10, timeinfo.tm_hour % 10};
  std::vector<int> mm = {(timeinfo.tm_min - timeinfo.tm_min % 10) / 10, timeinfo.tm_min % 10};

  // Clear leading zeroes for hour values less then 10
  int hourPosX = 0;
  if (hh.at(0) == 0){
    hh.erase(hh.begin());
    hourPosX = COLS / 2;
    Screen.clear();
  }

  // Draw on screen
  Screen.drawBigNumbers(hourPosX, 0, hh, Screen.getCurrentBrightness());
  Screen.drawBigNumbers(0, ROWS / 2, mm, Screen.getCurrentBrightness());

  // Clear vectors
  hh.clear();
  mm.clear();

  // Update previous values
  previousHour = timeinfo.tm_hour;
  previousMinutes = timeinfo.tm_min;
}

const char *BigClockPlugin::getName() const
{
  return "Big Clock";
}
