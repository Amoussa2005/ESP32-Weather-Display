
#pragma once

#include <Arduino.h>

// Data representing a Lebanese city
struct City {
  const char* name;
  float latitude;
  float longitude;
};

// Weather information received from the API
struct WeatherData {
  float temperature;
  int humidity;
  float windSpeed;
  int weatherCode;
  bool valid;
};

// Retrieve weather information for a city
bool fetchWeather(const City& city, WeatherData& weather);

// Convert an API weather code into readable text
const char* getWeatherDescription(int weatherCode);
