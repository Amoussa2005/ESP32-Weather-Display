
#include "OLEDDisplay.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==============================
// OLED CONFIGURATION
// ==============================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI 23
#define OLED_SCK  18
#define OLED_CS   5
#define OLED_DC   22
#define OLED_RST  4

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &SPI,
  OLED_DC,
  OLED_RST,
  OLED_CS,
  10000000UL
);

// ==============================
// INITIALIZE OLED
// ==============================

void initOLED() {

  SPI.begin(
    OLED_SCK,
    -1,
    OLED_MOSI,
    OLED_CS
  );

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {

    Serial.println("OLED initialization failed");

    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

// ==============================
// STATUS SCREEN
// ==============================

void showMessage(
  const char* line1,
  const char* line2
) {

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 18);
  display.println(line1);

  display.setCursor(0, 34);
  display.println(line2);

  display.display();
}

// ==============================
// WEATHER SCREEN
// ==============================

void showWeather(
  const City& city,
  const WeatherData& weather
) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // City name
  display.setCursor(0, 0);
  display.print(city.name);

  // Separator
  display.drawLine(
    0, 11,
    127, 11,
    SSD1306_WHITE
  );

  if (!weather.valid) {

    display.setCursor(0, 25);
    display.println("Weather unavailable");

    display.setCursor(0, 40);
    display.println("Check API / WiFi");

    display.display();
    return;
  }

  // Temperature
  display.setTextSize(2);
  display.setCursor(0, 15);

  display.print(weather.temperature, 1);
  display.write(247);
  display.print("C");

  // Other weather measurements
  display.setTextSize(1);

  display.setCursor(0, 37);
  display.print("Humidity: ");
  display.print(weather.humidity);
  display.print("%");

  display.setCursor(0, 47);
  display.print("Wind: ");
  display.print(weather.windSpeed, 1);
  display.print(" km/h");

  display.setCursor(0, 57);
  display.print(
    getWeatherDescription(weather.weatherCode)
  );

  display.display();
}
