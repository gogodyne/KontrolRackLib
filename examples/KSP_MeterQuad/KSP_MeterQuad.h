// (c) 2025 gogodyne
#ifndef KSP_MeterQuad_h
#define KSP_MeterQuad_h

#include <KontrolRack.h>
#include <KontrolRack_KR_LED24OLED12864.h>

using namespace KontrolRack;

////////////////////////////////////////////////////////////////////////////////
class KSP_MeterQuad : public KR::MeterQuad
{
public:
  using Parent = KR::MeterQuad;

  // Data for display
  struct UnitData
  {
    uint8_t level = 0;
  };
  UnitData unitDatas[(uint8_t)KR::BankUnitCount::Quad];

  KSP_MeterQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin()
  {
    Parent::begin(30, false, SWITCH_ADDRESS_METER, OLED12864_ADDRESS, LED24_ADDRESS, EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));

    // Set the Module/Unit draw callbacks
    unitInfos[0].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
    unitInfos[1].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
    unitInfos[2].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
    unitInfos[3].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
  }

  virtual void loop(bool dirtyByEncoder = true) override
  {
    Parent::loop(dirtyByEncoder);

    // Input...
    {
      // Encoder click
      if (encBtn.btn.didPress)
      {
        // set mode
        cycleModuleMode();
        setEncRange();
      }

      // Encoder changed
      if (encBtn.encDelta)
      {
        // change to select/edit?
        if (moduleMode == KR::ModuleMode::Normal)
        {
          // set mode
          setModuleMode(KR::ModuleMode::Select);
          setEncRange();
        }
        else
        {
          if (moduleMode == KR::ModuleMode::Select)
          {
            setUnitSelected(encBtn.enc.slider.position);
          }
          else
          if (moduleMode == KR::ModuleMode::Edit)
          {
            setUnitLevel(encBtn.enc.slider.position);
          }
        }
      }
    }
  }

  virtual void setEncRange()
  {
      if (moduleMode == KR::ModuleMode::Select)
      {
        encBtn.enc.slider.set(0, getUnitCount() - 1, false, unitSelected);
      }
      else
      if (moduleMode == KR::ModuleMode::Edit)
      {
        encBtn.enc.slider.set(0, led24.getSize() - 1, false, unitDatas[unitSelected].level);
      }
  }

  virtual uint8_t setUnitLevel(int8_t level)
  {
    resetHighlightTimeout();
    resetModuleModeTimeout();

    unitDatas[unitSelected].level = constrain(level, 0, led24.getSize() - 1);

    return unitDatas[unitSelected].level;
  }

  virtual uint8_t cycleUnitLevel(int8_t delta)
  {
    setUnitLevel(unitDatas[unitSelected].level + delta);

    return unitDatas[unitSelected].level;
  }

  //------------------------------------------------------------------------------
  // Drawing

  // Meter Quad
  virtual void onDrawMeterQuad_Index(uint8_t index)
  {
    uint8_t unitLevel = unitDatas[index].level;

    // OLED
    {
      oled12864.clear();
      drawOledHighlight(index);

      oled12864.gfx.setTextSize(2);
      oled12864.gfx.printf("#%d\n", index);

      oled12864.render();
    }

    // LED24
    {
      led24.clear();
      drawLedHighlight(index);

      {
        if (unitLevel > 0)
        {
          led24.setBar(unitLevel - 1, LED_GREEN);
        }
        if (unitLevel < led24.getSize())
        {
          led24.setBar(unitLevel, LED_YELLOW);
        }
        if (unitLevel < led24.getSize() - 1)
        {
          led24.setBar(unitLevel + 1, LED_RED);
        }
      }

      led24.render();
    }
  }

  virtual void onDrawMeterQuad_Time(uint8_t index)
  {
    uint8_t unitLevel = unitDatas[index].level;

    // OLED
    {
      oled12864.clear();
      drawOledHighlight(index);

      oled12864.gfx.printf("\n%d", (int)timing.ms);

      oled12864.render();
    }

    // LED24
    {
      led24.clear();
      drawLedHighlight(index);

      {
        if (unitLevel > 0)
        {
          led24.setBar(unitLevel - 1, LED_GREEN);
        }
        if (unitLevel < led24.getSize())
        {
          led24.setBar(unitLevel, LED_YELLOW);
        }
        if (unitLevel < led24.getSize() - 1)
        {
          led24.setBar(unitLevel + 1, LED_RED);
        }
      }

      led24.render();
    }
  }

  virtual void onDrawMeterQuad_Level(uint8_t index)
  {
    uint8_t unitLevel = unitDatas[index].level;

    // OLED
    {
      oled12864.clear();
      drawOledHighlight(index);

      {
        oled12864.gfx.setTextSize(6);
        oled12864.gfx.printf("%02d", unitLevel);
        oled12864.gfx.setTextSize(2);
        oled12864.gfx.printf("#%d\n", index);

        oled12864.gfx.setTextSize(6);
        oled12864.gfx.print("  ");
        oled12864.gfx.setTextSize(2);
        oled12864.gfx.printf("%3d%%\n", (uint8_t)(((float)unitLevel / ((float)led24.getSize() - 1)) * 100));

        oled12864.gfx.setTextSize(6);
        oled12864.gfx.print("  ");
        oled12864.gfx.setTextSize(2);
        oled12864.gfx.printf("%03.1f", timing.fpsEstimate);

        oled12864.gfx.printf("\n%d", (int)timing.ms);
      }

      oled12864.render();
    }

    // LED24
    {
      led24.clear();
      drawLedHighlight(index);

      {
        if (unitLevel > 0)
        {
          led24.setBar(unitLevel - 1, LED_GREEN);
        }
        if (unitLevel < led24.getSize())
        {
          led24.setBar(unitLevel, LED_YELLOW);
        }
        if (unitLevel < led24.getSize() - 1)
        {
          led24.setBar(unitLevel + 1, LED_RED);
        }
      }

      led24.render();
    }
  }
};

#endif// KSP_MeterQuad_h
