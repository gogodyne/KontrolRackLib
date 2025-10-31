// (c) 2025 gogodyne
#ifndef KontrolRack_KR_LED24OLED12864_h
#define KontrolRack_KR_LED24OLED12864_h

#include "KontrolRack_KR.h"
#include "KontrolRack_SSD1306.h"
#include "KontrolRack_LED24.h"

namespace KontrolRack {

namespace KR {

////////////////////////////////////////////////////////////////////////////////
// This Module is an array of meters, with screens for labels.
//
// - Rotary Encoder; +/-, pushbutton
// - I2C MUX TCA9548A; to switch between array banks
// Array bank:
// - OLED 128x64 (I2C); bank label
// - Adafruit 24-segment Bargraph (I2C); bank meter
class LED24OLED12864 : public ModuleI2C
{
public:
  using Parent = ModuleI2C;

  OLED12864 oled12864;
  Bank::Device* oled12864Devices = nullptr;
  bool* oled12864Inverted = nullptr;
  bool* led24Blink = nullptr;

  LED24 led24;
  Bank::Device* led24Devices = nullptr;

  LED24OLED12864(TwoWire& inWire)
  : Parent(inWire)
  , oled12864(inWire)
  {}

  using Parent::begin;
  virtual void begin(fps_t fps, bool test, uint8_t switchAddress, uint8_t oledAddress, LED24::Info led24Info, EncBtn::Info encInfo)
  {
    Parent::begin(fps, test, switchAddress, encInfo);

    // Init I2C devices.
    for (int i = 0; i < getBankCount(); ++i)
    {
      openBankPorts(i);
      // Init OLED.
      {
        oled12864.begin(oledAddress);
        if (test)
        {
          oled12864.test(i);
          oled12864.render();
        }
      }
      // Init LED.
      {
        led24.begin(led24Info);
        led24.flip = true;
        if (test)
        {
          led24.test(i);
          led24.render();
        }
      }
    }
  }

  virtual void loopDevices() override
  {
    for (int i = 0; i < getBankCount(); ++i)
    {
      if (led24Devices) led24Devices[i].loop();
      if (oled12864Devices) oled12864Devices[i].loop();
    }
  }

  virtual void drawOledEffects(uint8_t bankIndex)
  {
    if (bankSelectMode == BankSelectMode::None)
    {
      // screen saver
      drawOledInverted(bankIndex, oled12864Devices[bankIndex].timing.isHz(1.f/30.f));
    }
    else
    {
      drawOledHighlight(bankIndex);
    }
  }

  virtual void drawOledHighlight(uint8_t bankIndex)
  {
    bool isSelected = (bankIndex == bankSelectedIndex) && (bankSelectMode != BankSelectMode::None);
    bool isEdit = (bankSelectMode == BankSelectMode::Edit);

    // selection highlight
    if (isSelected && (isEdit || timing.isHz(2) || (bankHighlightTimeout > timing.ms)))
    {
      drawOledInverted(bankIndex, true);
    }
    else
    {
      drawOledInverted(bankIndex, false);
    }
  }

  virtual void drawOledInverted(uint8_t bankIndex, bool invert)
  {
    if (oled12864Inverted)
    {
      // Only if changing.
      if (oled12864Inverted[bankIndex] != invert)
      {
        oled12864Inverted[bankIndex] = invert;

        oled12864.gfx.invertDisplay(oled12864Inverted[bankIndex]);
      }
    }
  }

  virtual void drawLed24Effects(uint8_t bankIndex)
  {
    if (bankSelectMode == BankSelectMode::None)
    {
      drawLed24Blink(bankIndex, false);
    }
    else
    {
      drawLed24Highlight(bankIndex);
    }
  }

  virtual void drawLed24Highlight(uint8_t bankIndex)
  {
    bool isSelected = (bankIndex == bankSelectedIndex) && (bankSelectMode != BankSelectMode::None);
    bool isEdit = (bankSelectMode == BankSelectMode::Edit);
    bool blink = isSelected && isEdit;

    drawLed24Blink(bankIndex, blink);
  }

  virtual void drawLed24Blink(uint8_t bankIndex, bool blink)
  {
    if (led24Blink)
    {
      // Only if changing.
      if (led24Blink[bankIndex] != blink)
      {
        led24Blink[bankIndex] = blink;

        led24.blinkDisplay(led24Blink[bankIndex]);
      }
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
// A meter display array module.
// This class instantiates the desired number of arrays.
template <BankSize BANKCOUNT>
class Meter24 : public LED24OLED12864
{
public:
  static const uint8_t bankCount = (uint8_t)BANKCOUNT;
  Bank _banks[bankCount];

  Bank::Device _oled12864Devices[bankCount];
  bool _oled12864Inverted[bankCount];
  bool _led24Blink[bankCount];

  Bank::Device _led24Devices[bankCount];

  Meter24(TwoWire& inWire)
  : LED24OLED12864(inWire)
  {
    banks = _banks;

    oled12864Devices = _oled12864Devices;
    oled12864Inverted = _oled12864Inverted;
    led24Blink = _led24Blink;

    led24Devices = _led24Devices;
  }

  virtual uint8_t getBankCount() const override { return (uint8_t)BANKCOUNT; }
};

////////////////////////////////////////////////////////////////////////////////
// Pre-sized Module types.
typedef Meter24<BankSize::Mono> Meter24Mono;
typedef Meter24<BankSize::Dual> Meter24Dual;
typedef Meter24<BankSize::Quad> Meter24Quad;

}// namespace KR

}// namespace KontrolRack

#endif// KontrolRack_KR_LED24OLED12864_h
