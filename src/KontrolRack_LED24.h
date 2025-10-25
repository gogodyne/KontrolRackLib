// (c) 2025 gogodyne
#ifndef KontrolRack_LED24_h
#define KontrolRack_LED24_h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#define AdafruitBargraph_Address 0x70 // 0x70-0x77

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
class LED24
{
public:
  ////////////////////////////////////////////////////////////////////////////////
  struct Info
  {
    uint8_t i2cAddress = 0;
    TwoWire* wire = nullptr;
    uint8_t brightness = 15;// [0-15]

    Info() {}
    Info(uint8_t i2cAddress, TwoWire* wire, uint8_t brightness)
    : i2cAddress(i2cAddress)
    , wire(wire)
    , brightness(brightness)
    {}
  };

  Info info;
  Adafruit_24bargraph driver = Adafruit_24bargraph();
  bool useBlink = false;
  bool flip = false;

  virtual void begin(Info infoIn)
  {
    info = infoIn;

    driver.begin(info.i2cAddress, info.wire);
    // [0-15]
    driver.setBrightness(info.brightness);
  }

  static uint8_t getSize() { return 24; }

  virtual void setBar(uint8_t bar, uint8_t color)
  {
    driver.setBar(flipBar(bar), color);
  }

  virtual void clear()
  {
    driver.clear();
  }

  virtual void render()
  {
    driver.writeDisplay();
  }

  virtual uint8_t flipBar(uint8_t bar)
  {
    return flip ? ((getSize() - 1) - bar) : bar;
  }

  virtual void blinkDisplay(bool on)
  {
    // if (useBlink != on)
    // {
      useBlink = on;
      driver.blinkRate(useBlink ? HT16K33_BLINK_2HZ : HT16K33_BLINK_OFF);
    // }
  }

  virtual void test(int value)
  {
      setBar(value + 0, LED_RED);
      setBar(value + 1, LED_YELLOW);
      setBar(value + 2, LED_GREEN);
  }
};

}// namespace KontrolRack

#endif// KontrolRack_LED24_h
