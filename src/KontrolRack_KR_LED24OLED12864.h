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
// - I2C MUX TCA9548A; to switch between array units
// Array unit:
// - OLED 128x64 (I2C); unit label
// - Adafruit 24-segment Bargraph (I2C); unit meter
class LED24OLED12864 : public Module
{
public:
  using Parent = Module;

  OLED12864 oled12864;
  LED24 led24;

  LED24OLED12864(TwoWire& inWire)
  : Parent(inWire)
  , oled12864(inWire)
  {}

  using Parent::begin;
  virtual void begin(uint8_t switchAddress, uint8_t oledAddress, uint8_t led24Address, EncBtn::Info encInfo, fps_t fps, bool test)
  {
    Parent::begin(switchAddress, encInfo, fps, test);

    // init I2C devices
    for (int i = 0; i < getUnitCount(); ++i)
    {
      openUnitPorts(i);
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

  virtual void drawOledHighlight(uint8_t index)
  {
    bool isSelected = (index == unitSelected) && (moduleMode != ModuleMode::Normal);
    bool isEdit = (moduleMode == ModuleMode::Edit);
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
    bool isSelected = (index == unitSelected) && (moduleMode != ModuleMode::Normal);
    bool isEdit = (moduleMode == ModuleMode::Edit);
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
template <BankUnitCount UNITCOUNT>
class Meter24 : public LED24OLED12864
{
public:
  UnitInfo unitInfos[(uint8_t)UNITCOUNT];

  Meter24(TwoWire& inWire)
  : LED24OLED12864(inWire)
  {
    _unitInfos = unitInfos;
  }

  virtual uint8_t getUnitCount() const override { return (uint8_t)UNITCOUNT; }
};

////////////////////////////////////////////////////////////////////////////////
// Unit arrays
typedef Meter24<BankUnitCount::Mono> MeterMono;
typedef Meter24<BankUnitCount::Dual> MeterDual;
typedef Meter24<BankUnitCount::Quad> MeterQuad;
typedef Meter24<BankUnitCount::Octo> MeterOcto;

}// namespace KR

}// namespace KontrolRack

#endif// KontrolRack_KR_LED24OLED12864_h
