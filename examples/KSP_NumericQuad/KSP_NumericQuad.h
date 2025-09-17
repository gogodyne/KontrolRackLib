// (c) 2025 gogodyne
#ifndef KSP_NumericQuad_h
#define KSP_NumericQuad_h

#include <KontrolRack.h>
#include <KontrolRack_KR_Num8OLED12864.h>

using namespace KontrolRack;

////////////////////////////////////////////////////////////////////////////////
class KSP_NumericQuad : public KR::NumericQuad
{
public:
  using Parent = KR::NumericQuad;

  // Data for display
  struct UnitData
  {
    uint8_t level = 0;
  };
  UnitData unitDatas[(uint8_t)KR::BankUnitCount::Quad];

  KSP_NumericQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin()
  {
    Parent::begin(SWITCH_ADDRESS, OLED12864_ADDRESS, Num8::Info(NUM8_DIN, NUM8_CS, NUM8_CLK, NUM8_INTENSITY), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S), 30, false);

    // Set the Module/Unit draw callbacks
    unitInfos[0].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
    unitInfos[1].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
    unitInfos[2].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
    unitInfos[3].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
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
        encBtn.enc.slider.set(0, LEVEL_RANGE - 1, false, unitDatas[unitSelected].level);
      }
  }

  virtual uint8_t setUnitLevel(int8_t level)
  {
    resetHighlightTimeout();
    resetModuleModeTimeout();

    unitDatas[unitSelected].level = constrain(level, 0, LEVEL_RANGE - 1);

    return unitDatas[unitSelected].level;
  }

  virtual uint8_t cycleUnitLevel(int8_t delta)
  {
    setUnitLevel(unitDatas[unitSelected].level + delta);

    return unitDatas[unitSelected].level;
  }

  //------------------------------------------------------------------------------
  // Drawing

  // Numeric Quad
  virtual void onDrawNumericQuad_Index(uint8_t index)
  {
    // OLED
    {
      oled12864.clear();

      oled12864.gfx.setTextSize(8);
      oled12864.gfx.printf("%d!", index);

      drawOledHighlight(index);
      oled12864.render();
    }
  }
  
  virtual void onDrawNumericQuad_Time(uint8_t index)
  {
    // OLED
    {
      oled12864.clear();

      oled12864.gfx.setTextSize(2);
      // oled12864.gfx.printf("%d", (int)timing.ms);
      oled12864.gfx.print("\nTime(ms)");

      drawOledHighlight(index);
      oled12864.render();
    }

    // Numeric LED
    {
      // Numeric is not I2C; clear is done in the draw

      num8.printUnit(index, timing.ms);

      drawNumHighlight(index);
      // Numeric is not I2C; render is done in the draw
    }
  }
  
  virtual void onDrawNumericQuad_Level(uint8_t index)
  {
    // OLED
    {
      oled12864.clear();

      oled12864.gfx.setTextSize(4);
      oled12864.gfx.printf("%d\n", unitDatas[index].level);
      oled12864.gfx.setTextSize(2);
      oled12864.gfx.printf("%03.1f", timing.fpsEstimate);

      drawOledHighlight(index);
      oled12864.render();
    }

    // Numeric LED
    {
      // Numeric is not I2C; clear is done in the draw

      num8.printUnit(index, unitDatas[index].level);

      drawNumHighlight(index);
      // Numeric is not I2C; render is done in the draw
    }
  }
};

#endif// KSP_NumericQuad_h
