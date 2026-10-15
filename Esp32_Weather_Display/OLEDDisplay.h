
#pragma once

#include <Arduino.h>
#include "WeatherAPI.h"

// Initialize SPI OLED
void initOLED();

// Display status messages
void showMessage(
  const char* line1,
  const char* line2 = ""
);

// Display weather data
void showWeather(
  const City& city,
  const WeatherData& weather
);
