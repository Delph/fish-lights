#ifndef MAIN_H_INCLUDE
#define MAIN_H_INCLUDE

#include <Wire.h>

#define NO_ADAFRUIT_SSD1306_COLOR_COMPATIBILITY
#include <Adafruit_SSD1306.h>

#include "ds3231.h"

extern Adafruit_SSD1306 display;
extern DS3231 rtc;

enum class ClockSetUnit
{
  YEAR,
  MONTH,
  DAY,
  HOUR,
  MINUTE,
  SECOND
};

void display_clock();
void display_clock_select(const ClockSetUnit unit);

#endif
