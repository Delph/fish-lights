#include "main.h"

Adafruit_SSD1306 display(128, 32, &Wire, -1);
DS3231 rtc;


void display_clock()
{
  DateTime dt = rtc.getDateTimeDST();
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(dt.year);
  display.print("-");
  if (dt.month < 10)
    display.print("0");
  display.print(dt.month);
  display.print("-");
  if (dt.day < 10)
    display.print("0");
  display.print(dt.day);
  display.print(" ");
  if (dt.hours < 10)
    display.print("0");
  display.print(dt.hours);
  display.print(":");
  if (dt.minutes < 10)
    display.print("0");
  display.print(dt.minutes);
  display.print(":");
  if (dt.seconds < 10)
    display.print("0");
  display.print(dt.seconds);
}


void display_clock_select(const ClockSetUnit unit)
{
  switch (unit)
  {
    case ClockSetUnit::YEAR:
      display.drawFastHLine(0, 8, 24, 1);
    break;
    case ClockSetUnit::MONTH:
      display.drawFastHLine(30, 8, 12, 1);
    break;
    case ClockSetUnit::DAY:
      display.drawFastHLine(48, 8, 12, 1);
    break;
    case ClockSetUnit::HOUR:
      display.drawFastHLine(64, 8, 12, 1);
    break;
    case ClockSetUnit::MINUTE:
      display.drawFastHLine(82, 8, 12, 1);
    break;
    case ClockSetUnit::SECOND:
      display.drawFastHLine(100, 8, 12, 1);
    break;
  }
}
