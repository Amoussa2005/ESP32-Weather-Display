
#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "WeatherAPI.h"
#include "OLEDDisplay.h"

// ==============================
// BUTTON CONFIGURATION
// ==============================

#define BUTTON_NEXT 32
#define BUTTON_PREV 33

#define DEBOUNCE_DELAY 50UL

// ==============================
// LEBANESE CITIES
// ==============================

const City cities[] = {

  {"Beirut",      33.8938, 35.5018},
  {"Tripoli",     34.4335, 35.8441},
  {"Sidon",       33.5630, 35.3688},
  {"Tyre",        33.2705, 35.2038},
  {"Zahle",       33.8463, 35.9020},
  {"Baalbek",     34.0058, 36.2181},
  {"Nabatieh",    33.3789, 35.4839},
  {"Jounieh",     33.9808, 35.6178},

  {"Byblos",      34.1236, 35.6511},
  {"Batroun",     34.2553, 35.6581},
  {"Zgharta",     34.3974, 35.8957},
  {"Bcharre",     34.2508, 36.0106},
  {"Aley",        33.8070, 35.5970},
  {"Bhamdoun",    33.8083, 35.6580},
  {"Chouf",       33.6950, 35.5850},
  {"Jezzine",     33.5417, 35.5844},

  {"Marjayoun",   33.3606, 35.5928},
  {"Hasbaya",     33.3977, 35.6856},
  {"Rashaya",     33.4990, 35.8380},
  {"Hermel",      34.3948, 36.3858},
  {"Halba",       34.5442, 36.0790},
  {"Amchit",      34.1490, 35.6440},
  {"Bint Jbeil",  33.1180, 35.4330},
  {"Broummana",   33.8844, 35.6331}

};

// Automatically calculate city count
const int CITY_COUNT =
  sizeof(cities) / sizeof(cities[0]);

int currentCity = 0;

WeatherData currentWeather = {};

unsigned long lastWeatherUpdate = 0;

// ==============================
// BUTTON STATES
// ==============================

struct ButtonState {

  uint8_t pin;
  bool lastReading;
  bool stableState;
  unsigned long lastChange;

};

ButtonState nextButton = {
  BUTTON_NEXT, HIGH, HIGH, 0
};

ButtonState prevButton = {
  BUTTON_PREV, HIGH, HIGH, 0
};

// ==============================
// WIFI CONNECTION
// ==============================

bool connectWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  showMessage(
    "Connecting WiFi",
    "Please wait..."
  );

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - startTime < 15000UL
  ) {

    delay(250);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    return true;
  }

  Serial.println("WiFi connection failed");

  return false;
}

// ==============================
// UPDATE WEATHER
// ==============================

void updateWeather() {

  const City& city = cities[currentCity];

  Serial.println();
  Serial.print("Selected city: ");
  Serial.println(city.name);

  showMessage(
    city.name,
    "Loading weather..."
  );

  currentWeather.valid = false;

  if (connectWiFi()) {

    if (!fetchWeather(city, currentWeather)) {
      Serial.println("Weather request failed");
    }

  } else {

    Serial.println("No WiFi connection");
  }

  showWeather(
    city,
    currentWeather
  );

  lastWeatherUpdate = millis();
}

// ==============================
// BUTTON DEBOUNCING
// ==============================

bool buttonPressed(ButtonState& button) {

  bool reading = digitalRead(button.pin);

  if (reading != button.lastReading) {
    button.lastChange = millis();
  }

  if (
    millis() - button.lastChange > DEBOUNCE_DELAY
  ) {

    if (reading != button.stableState) {

      button.stableState = reading;

      if (button.stableState == LOW) {

        button.lastReading = reading;
        return true;
      }
    }
  }

  button.lastReading = reading;

  return false;
}

// ==============================
// SETUP
// ==============================

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);

  initOLED();

  showMessage(
    "ESP32 Weather",
    "Starting..."
  );

  delay(1000);

  connectWiFi();

  updateWeather();
}

// ==============================
// MAIN LOOP
// ==============================

void loop() {

  // Navigate to next city
  if (buttonPressed(nextButton)) {

    currentCity =
      (currentCity + 1) % CITY_COUNT;

    updateWeather();
  }

  // Navigate to previous city
  if (buttonPressed(prevButton)) {

    currentCity =
      (currentCity - 1 + CITY_COUNT) % CITY_COUNT;

    updateWeather();
  }

  // Refresh weather automatically
  if (
    millis() - lastWeatherUpdate >= WEATHER_REFRESH_MS
  ) {

    updateWeather();
  }

  delay(5);
}
