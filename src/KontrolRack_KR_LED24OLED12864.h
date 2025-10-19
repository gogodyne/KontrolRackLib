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
  LED24 led24;
  Bank::Device* _led24Devices = nullptr;
  Bank::Device* _oled12864Devices = nullptr;

  LED24OLED12864(TwoWire& inWire)
  : Parent(inWire)
  , oled12864(inWire)
  {}

  using Parent::begin;
  virtual void begin(fps_t fps, bool test, uint8_t switchAddress, uint8_t oledAddress, uint8_t led24Address, EncBtn::Info encInfo)
  {
    Parent::begin(fps, test, switchAddress, encInfo);

    // init I2C devices
    for (int i = 0; i < getBankSize(); ++i)
    {
      openBankPorts(i);
      // init OLED
      {
        oled12864.begin(oledAddress);
        if (test)
        {
          oled12864.test(i);
          oled12864.render();
        }
      }
      // init LED
      {
        led24.begin(led24Address, wire);
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
    for (int i = 0; i < getBankSize(); ++i)
    {
      if (_led24Devices) _led24Devices[i].loop();
      if (_oled12864Devices) _oled12864Devices[i].loop();
    }
  }

  virtual void drawOledHighlight(uint8_t index)
  {
    bool isSelected = (index == bankSelected) && (bankSelectMode != BankSelectMode::Normal);
    bool isEdit = (bankSelectMode == BankSelectMode::Edit);
    bool isHighlight = highlightTimeout > timing.ms;

    // selection highlight
    if (isSelected && (isHighlight || isEdit || timing.isHz(2)))
    {
      oled12864.gfx.invertDisplay(true);
    }
    else
    {
      oled12864.gfx.invertDisplay(false);
    }
  }

  virtual void drawLedHighlight(uint8_t index)
  {
    bool isSelected = (index == bankSelected) && (bankSelectMode != BankSelectMode::Normal);
    bool isEdit = (bankSelectMode == BankSelectMode::Edit);
    bool isHighlight = highlightTimeout > timing.ms;

    // Edit indicator
    if (isSelected && isEdit && !isHighlight)
    {
      led24.blinkDisplay(true);
    }
    else
    {
      led24.blinkDisplay(false);
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
  Bank banks[(uint8_t)BANKCOUNT];
  Bank::Device led24Devices[(uint8_t)BANKCOUNT];
  Bank::Device oled12864Devices[(uint8_t)BANKCOUNT];

  Meter24(TwoWire& inWire)
  : LED24OLED12864(inWire)
  {
    _banks = banks;
    _led24Devices = led24Devices;
    _oled12864Devices = oled12864Devices;
  }

  virtual uint8_t getBankSize() const override { return (uint8_t)BANKCOUNT; }
};

////////////////////////////////////////////////////////////////////////////////
// Pre-sized Module types.
typedef Meter24<BankSize::Mono> Meter24Mono;
typedef Meter24<BankSize::Dual> Meter24Dual;
typedef Meter24<BankSize::Quad> Meter24Quad;

}// namespace KR

}// namespace KontrolRack

#endif// KontrolRack_KR_LED24OLED12864_h
