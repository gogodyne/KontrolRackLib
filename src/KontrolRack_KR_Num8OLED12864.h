// (c) 2025 gogodyne
#ifndef KontrolRack_KR_Num8OLED12864_h
#define KontrolRack_KR_Num8OLED12864_h

#include <functional>
#include "KontrolRack_KR.h"
#include "KontrolRack_SSD1306.h"
#include "KontrolRack_MAX7219.h"

namespace KontrolRack {

namespace KR {

////////////////////////////////////////////////////////////////////////////////
// This Module is an array of numeric displays, with screens for labels.
//
// - Rotary Encoder; +/-, pushbutton
// - I2C MUX TCA9548A; to switch between array banks
// Array bank:
// - OLED 128x64 (I2C); bank label
// - MAX7219 8-digit 7-segment display; bank number
class Num8OLED12864 : public ModuleI2C
{
public:
  using Parent = ModuleI2C;

  OLED12864 oled12864;
  Bank::Device* _oled12864Devices = nullptr;
  bool* _oled12864Inverted = nullptr;
  Num8 num8;
  Bank::Device num8Device;

  Num8OLED12864(TwoWire& inWire)
  : Parent(inWire)
  , oled12864(inWire)
  {}

  using Parent::begin;
  virtual void begin(fps_t fps, bool test, uint8_t switchAddress, uint8_t oledAddress, Num8::Info num8Info, EncBtn::Info encInfo)
  {
    Parent::begin(fps, test, switchAddress, encInfo);

    // Init I2C devices.
    for (int i = 0; i < getBankSize(); ++i)
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
    }

    // Init Numeric.
    {
      num8.begin(num8Info);
      num8.clear();
      if (test)
      {
        num8.test(millis());
        num8.render();
      }
    }
  }

  virtual void loopDevices() override
  {
    num8Device.loop();

    for (int i = 0; i < getBankSize(); ++i)
    {
      if (_oled12864Devices) _oled12864Devices[i].loop();
    }
  }

  virtual void drawBanks(bool isDirty) override
  {
    // Numeric is not I2C; clear/render once
    if (isDirty || num8Device.timing.isTick)
      num8.clear();

    Parent::drawBanks(isDirty);

    if (isDirty || num8Device.timing.isTick)
      num8.render();
  }

  virtual void drawBankInverted(uint8_t bankIndex, bool invert)
  {
    if (_oled12864Inverted)
    {
      // Only if changing.
      if (_oled12864Inverted[bankIndex] != invert)
      {
        _oled12864Inverted[bankIndex] = invert;

        oled12864.gfx.invertDisplay(_oled12864Inverted[bankIndex]);
      }
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

  virtual void drawNumHighlight(uint8_t index)
  {
    // bool isSelected = (index == bankSelected) && (bankSelectMode != BankSelectMode::Normal);
    // bool isEdit = (bankSelectMode == BankSelectMode::Edit);
    // bool isHighlight = highlightTimeout > timing.ms;

    // Edit indicator
    // if (isSelected && isEdit && !isHighlight)
    // {
    //   led24.blinkDisplay(true);
    // }
    // else
    // {
    //   led24.blinkDisplay(false);
    // }
  }
};

////////////////////////////////////////////////////////////////////////////////
// A numeric display array module.
// This class layer instantiates the desired number of arrays.
template <BankSize BANKCOUNT>
class Numeric8 : public Num8OLED12864
{
public:
  Bank banks[(uint8_t)BANKCOUNT];

  Bank::Device oled12864Devices[(uint8_t)BANKCOUNT];
  bool oled12864Inverted[(uint8_t)BANKCOUNT];

  Numeric8(TwoWire& inWire)
  : Num8OLED12864(inWire)
  {
    _banks = banks;

    _oled12864Devices = oled12864Devices;
    _oled12864Inverted = oled12864Inverted;
  }

  virtual uint8_t getBankSize() const override { return (uint8_t)BANKCOUNT; }
};

////////////////////////////////////////////////////////////////////////////////
// Pre-sized Module types.
typedef Numeric8<BankSize::Mono> Numeric8Mono;
typedef Numeric8<BankSize::Dual> Numeric8Dual;
typedef Numeric8<BankSize::Quad> Numeric8Quad;

}// namespace KR

}// namespace KontrolRack

#endif// KontrolRack_KR_Num8OLED12864_h
