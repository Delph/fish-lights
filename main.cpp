#include <Arduino.h>

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

#include <IRLremote.h>

#include "main.h"

const uint8_t PIN_IR = 2;
const uint8_t PIN_LEDS = 4;

const size_t NUM_LEDS = 12;
CRGB leds[NUM_LEDS];

const uint32_t CORRECTION = 0xffffff;

bool on = true;

enum Button
{
  NO_COMMAND = 1,
  ADDRESS = 65280,
  ON = 13,
  OFF = 31,
  BRIGHTNESS_DEC = 29,
  BRIGHTNESS_INC = 9,
  WHITE = 21,
  BLUE = 17,
  GREEN = 27,
  RED = 25,
  FLASH = 77,
  STROBE = 0,
  FADE = 26,
  SMOOTH = 12,
  LIGHT_BLUE = 22,
  LIME = 18,
  DARK_ORANGE = 23,
  DARK_PURPLE = 4,
  OLIVE = 76,
  ORANGE = 64,
  PURPLE = 14,
  TURQUOISE = 30,
  LIGHT_ORANGE = 10,
  PINK = 15,
  TEAL = 20,
  YELLOW = 28
};
Button solid = Button::WHITE;

CRGB mapColour(Button button)
{
  switch (button)
  {
    case Button::RED:
      return CRGB::Red;
    case Button::GREEN:
      return CRGB::Green;
    case Button::BLUE:
      return CRGB::Blue;
    case Button::WHITE:
      return CRGB::White;
    case Button::DARK_ORANGE:
      return CRGB::DarkOrange;
    case Button::LIME:
      return CRGB::LimeGreen;
    case Button::LIGHT_BLUE:
      return CRGB::LightBlue;
    case Button::ORANGE:
      return CRGB::Orange;
    case Button::OLIVE:
      return CRGB::Olive;
    case Button::DARK_PURPLE:
      return 0x4A235A; // some dark purple thing
    case Button::LIGHT_ORANGE:
      return 0xF5B041;
    case Button::TURQUOISE:
      return CRGB::Turquoise;
    case Button::PURPLE:
      return CRGB::Purple;
    case Button::YELLOW:
      return CRGB::Yellow;
    case Button::TEAL:
      return CRGB::Teal;
    case Button::PINK:
      return CRGB::Pink;
    default:
      return CRGB::Black;
  }
}

enum class LightMode
{
  SOLID,
  FLASH,
  STROBE,
  FADE,
  SMOOTH
};
LightMode lightMode = LightMode::SOLID;

enum class DisplayMode
{
  OFF,
  CLOCK,
  SET_CLOCK
};
DisplayMode displayMode = DisplayMode::OFF;


ClockSetUnit clockSetUnit = ClockSetUnit::YEAR;

CNec remote;
Button getIRCode()
{
  if (remote.available())
  {
    auto data = remote.read();

    digitalWrite(LED_BUILTIN, 1);
    delay(100);
    digitalWrite(LED_BUILTIN, 0);

    if (data.address == 65280)
      return static_cast<Button>(data.command);
  }

  return NO_COMMAND;
}


void setup()
{
  // setup display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
  {
    while (true)
    {
      digitalWrite(LED_BUILTIN, 1);
      delay(500);
      digitalWrite(LED_BUILTIN, 0);
      delay(500);
    }
  }
  display.clearDisplay();
  display.display();
  display.setTextColor(SSD1306_WHITE);

  // setup rtc
  if (!rtc.begin())
  {
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("RTC failed");
    display.display();

    while (true);
  }

  // add LEDs
  FastLED.addLeds<WS2812, PIN_LEDS, GRB>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness(63);

  // setup ir
  if (!remote.begin(PIN_IR))
  {
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("IR failed");
    display.display();

    while (true);
  }
}

uint32_t last = 0;
const uint32_t FPS = 100;
void loop()
{
  const uint32_t now = millis();
  if ((now - last) < 1000 / FPS)
    return;
  last = millis();

  const uint8_t code = getIRCode();
  switch (code)
  {
    case Button::OFF:
      if (displayMode != DisplayMode::SET_CLOCK)
        on = false;
    break;
    case Button::ON:
      if (displayMode == DisplayMode::SET_CLOCK)
      {
        if (clockSetUnit == ClockSetUnit::SECOND)
          displayMode = DisplayMode::CLOCK;
        else
          clockSetUnit = static_cast<ClockSetUnit>(static_cast<int>(clockSetUnit) + 1);
      }
      else
      {
        on = true;
      }
    break;
    case Button::BRIGHTNESS_INC:
      if (displayMode == DisplayMode::SET_CLOCK)
      {
        DateTime dt = rtc.getDateTime();
        switch (clockSetUnit)
        {
          case ClockSetUnit::YEAR:
            dt.year += 1;
          break;
          case ClockSetUnit::MONTH:
            dt.month += 1;
          break;
          case ClockSetUnit::DAY:
            dt.day += 1;
          break;
          case ClockSetUnit::HOUR:
            dt.hours += 1;
          break;
          case ClockSetUnit::MINUTE:
            dt.minutes += 1;
          break;
          case ClockSetUnit::SECOND:
            dt.seconds += 1;
          break;
        }
        dt.normalize();
        rtc.setDateTime(dt);
      }
      else
      {
        FastLED.setBrightness(min(FastLED.getBrightness() + 64, 255));
      }
    break;
    case Button::BRIGHTNESS_DEC:
      if (displayMode == DisplayMode::SET_CLOCK)
      {
        DateTime dt = rtc.getDateTime();
        switch (clockSetUnit)
        {
          case ClockSetUnit::YEAR:
            dt.year -= 1;
          break;
          case ClockSetUnit::MONTH:
            dt.month -= 1;
          break;
          case ClockSetUnit::DAY:
            dt.day -= 1;
          break;
          case ClockSetUnit::HOUR:
            dt.hours -= 1;
          break;
          case ClockSetUnit::MINUTE:
            dt.minutes -= 1;
          break;
          case ClockSetUnit::SECOND:
            dt.seconds -= 1;
          break;
        }
        dt.normalize();
        rtc.setDateTime(dt);
      }
      else
      {
        FastLED.setBrightness(max(FastLED.getBrightness() - 64, 63));
      }
    break;
    case Button::WHITE:
    case Button::BLUE:
    case Button::GREEN:
    case Button::RED:
    case Button::LIGHT_BLUE:
    case Button::LIME:
    case Button::DARK_ORANGE:
    case Button::DARK_PURPLE:
    case Button::OLIVE:
    case Button::ORANGE:
    case Button::PURPLE:
    case Button::TURQUOISE:
    case Button::LIGHT_ORANGE:
    case Button::PINK:
    case Button::TEAL:
    case Button::YELLOW:
      lightMode = LightMode::SOLID;
      solid = static_cast<Button>(code);
    break;
    case Button::FLASH:
      if (displayMode == DisplayMode::OFF)
      {
        displayMode = DisplayMode::CLOCK;
      }
      else if (displayMode == DisplayMode::CLOCK)
      {
        displayMode = DisplayMode::SET_CLOCK;
        clockSetUnit = ClockSetUnit::YEAR;
      }
      else
      {
        displayMode = DisplayMode::OFF;
      }
    break;
    case Button::STROBE:
      lightMode = LightMode::STROBE;
    break;
    case Button::FADE:
      lightMode = LightMode::FADE;
    break;
    case Button::SMOOTH:
      lightMode = LightMode::SMOOTH;
    break;
  }

  const DateTime dt = rtc.getDateTimeDST();
  if (on == false && dt.hours == 2)
    on = true;
  if (on && dt.hours >= 7 && dt.hours <= 22)
  {
    switch (lightMode)
    {
      case LightMode::SOLID:
        fill_solid(leds, NUM_LEDS, mapColour(solid));
      break;
      case LightMode::STROBE:
      {
        static uint16_t startIndex = 0;
        startIndex += 4;
        uint16_t colourIndex = startIndex / 10;
        for (size_t i = 0; i < NUM_LEDS; ++i)
        {
          leds[i] = ColorFromPalette(OceanColors_p, colourIndex, 255, LINEARBLEND);
          colourIndex += 3;
        }
      }
      break;
      case LightMode::FADE:
      {
        CHSV colours[] = {
          CHSV(HUE_PURPLE, 0, 127),
          CHSV(HUE_PURPLE, 0, 127),
          CHSV(HUE_AQUA, 255, 255),
          CHSV(HUE_AQUA, 255, 255),
          CHSV(HUE_PURPLE, 255, 255),
          CHSV(HUE_PURPLE, 255, 255),
          CHSV(HUE_AQUA, 0, 255),
          CHSV(HUE_AQUA, 0, 255)
        };

        const uint32_t period = 1000;

        for (size_t i = 0; i < NUM_LEDS; ++i)
        {
          const uint32_t now = (millis() + i * 200) % (8 * period); // number of colours
          const uint8_t progress = map(now % period, 0, 999, 0, 255);
          const uint32_t point = now / period;

          const CHSV current = colours[point];
          const CHSV next = colours[(point+1) % 8];
          leds[i] = CHSV(lerp8by8(current.h, next.h, progress),
            lerp8by8(current.s, next.s, progress),
            lerp8by8(current.v, next.v, progress)
          );
        }
      }
      break;
      case LightMode::SMOOTH:
      {
        for (size_t i = 0; i < NUM_LEDS; ++i)
          leds[i] = CHSV(inoise8(millis() / 12 + i*8), 255, 255);
      }
      break;
      default:
      break;
    }
  }
  else
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }

  FastLED.show();

  switch (displayMode)
  {
    case DisplayMode::OFF:
      display.clearDisplay();
    break;
    case DisplayMode::CLOCK:
      display_clock();
    break;
    case DisplayMode::SET_CLOCK:
      display_clock();
      display_clock_select(clockSetUnit);
    break;
  }
  display.display();
}
