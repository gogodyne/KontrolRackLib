// (c) 2025 gogodyne
#ifndef KontrolRack_SSD1306_h
#define KontrolRack_SSD1306_h

#include <Adafruit_SSD1306.h>

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
class SSD1306
{
public:
  Adafruit_SSD1306 driver;
  bool isBlink = false;
  Adafruit_GFX& gfx;

  SSD1306(uint8_t width, uint8_t height, TwoWire& inWire)
  : driver(128, 64, &inWire, -1)
  , gfx(driver)
  {}

  virtual void begin(uint8_t i2cAddr)
  {
    driver.begin(SSD1306_SWITCHCAPVCC, i2cAddr);// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    gfx.setTextColor(SSD1306_WHITE);
  }

  virtual void clear()
  {
    driver.clearDisplay();
    driver.setCursor(0, 0);
  }

  virtual void render()
  {
    driver.display();
  }

  virtual void blinkDisplay(bool on)
  {
    // if (isBlink != on)
    // {
      isBlink = on;
      gfx.invertDisplay(isBlink);
    // }
  }

  virtual void test(int value)
  {
    gfx.setCursor(0, 0);
    gfx.setTextSize(1);
    gfx.print(value);
    gfx.setTextSize(2);
    gfx.print(value);
    gfx.setTextSize(3);
    gfx.print(value);
    gfx.setTextSize(4);
    gfx.print(value);
  }
};

////////////////////////////////////////////////////////////////////////////////
class OLED12832 : public SSD1306
{
public:
  OLED12832(TwoWire& inWire)
  : SSD1306(128, 32, inWire)
  {}
};

////////////////////////////////////////////////////////////////////////////////
class OLED12864 : public SSD1306
{
public:
  OLED12864(TwoWire& inWire)
  : SSD1306(128, 64, inWire)
  {}
};

}// namespace KontrolRack

#endif// KontrolRack_SSD1306_h
