
#include "WeatherAPI.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

bool fetchWeather(const City& city, WeatherData& weather) {

  weather.valid = false;

  // Check Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[ERROR] Wi-Fi disconnected");
    return false;
  }

  // Construct the Open-Meteo URL
  String url = "https://api.open-meteo.com/v1/forecast";

  url += "?latitude=" + String(city.latitude, 4);
  url += "&longitude=" + String(city.longitude, 4);

  url += "&current=temperature_2m,";
  url += "relative_humidity_2m,";
  url += "weather_code,wind_speed_10m";

  url += "&timezone=Asia%2FBeirut";

  Serial.println();
  Serial.println("========================");
  Serial.print("City: ");
  Serial.println(city.name);
  Serial.print("URL: ");
  Serial.println(url);

  WiFiClientSecure client;

  // Development only: skip TLS certificate verification
  client.setInsecure();

  HTTPClient http;
  http.setTimeout(10000);

  if (!http.begin(client, url)) {
    Serial.println("[ERROR] HTTP initialization failed");
    return false;
  }

  // Perform GET request
  int httpCode = http.GET();

  Serial.print("HTTP status: ");
  Serial.println(httpCode);

  if (httpCode != HTTP_CODE_OK) {

    Serial.print("[ERROR] HTTP request failed: ");
    Serial.println(http.errorToString(httpCode));

    if (httpCode > 0) {
      Serial.println(http.getString());
    }

    http.end();
    return false;
  }

  // Read the decoded response body
  String payload = http.getString();

  Serial.println("API response:");
  Serial.println(payload);

  if (payload.length() == 0) {
    Serial.println("[ERROR] Empty response");
    http.end();
    return false;
  }

  // Parse the JSON text
  JsonDocument doc;

  DeserializationError error =
      deserializeJson(doc, payload);

  if (error) {

    Serial.print("[ERROR] JSON parsing: ");
    Serial.println(error.c_str());

    http.end();
    return false;
  }

  // Find the current weather object
  JsonVariant current = doc["current"];

  if (current.isNull() ||
      current["temperature_2m"].isNull() ||
      current["relative_humidity_2m"].isNull() ||
      current["wind_speed_10m"].isNull() ||
      current["weather_code"].isNull()) {

    Serial.println("[ERROR] Missing weather fields");

    http.end();
    return false;
  }

  // Extract weather values
  weather.temperature =
      current["temperature_2m"].as<float>();

  weather.humidity =
      current["relative_humidity_2m"].as<int>();

  weather.windSpeed =
      current["wind_speed_10m"].as<float>();

  weather.weatherCode =
      current["weather_code"].as<int>();

  weather.valid = true;

  Serial.println();
  Serial.println("WEATHER DATA:");

  Serial.print("Temperature: ");
  Serial.print(weather.temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(weather.humidity);
  Serial.println(" %");

  Serial.print("Wind speed: ");
  Serial.print(weather.windSpeed);
  Serial.println(" km/h");

  Serial.print("Condition: ");
  Serial.println(
    getWeatherDescription(weather.weatherCode)
  );

  Serial.println("========================");

  http.end();

  return true;
}


const char* getWeatherDescription(int code) {

  switch (code) {

    case 0:
      return "Clear sky";

    case 1:
      return "Mainly clear";

    case 2:
      return "Partly cloudy";

    case 3:
      return "Overcast";

    case 45:
    case 48:
      return "Foggy";

    case 51:
    case 53:
    case 55:
    case 56:
    case 57:
      return "Drizzle";

    case 61:
    case 63:
    case 65:
    case 66:
    case 67:
      return "Rain";

    case 71:
    case 73:
    case 75:
    case 77:
      return "Snow";

    case 80:
    case 81:
    case 82:
      return "Rain showers";

    case 85:
    case 86:
      return "Snow showers";

    case 95:
    case 96:
    case 99:
      return "Thunderstorm";

    default:
      return "Unknown";
  }
}
