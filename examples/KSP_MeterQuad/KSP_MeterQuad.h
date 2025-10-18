// (c) 2025 gogodyne
#ifndef KSP_MeterQuad_h
#define KSP_MeterQuad_h

#include <KontrolRack.h>
#include <KontrolRack_KR_LED24OLED12864.h>

using namespace KontrolRack;

////////////////////////////////////////////////////////////////////////////////
class KSP_MeterQuad : public KR::Meter24Quad
{
public:
  using Parent = KR::Meter24Quad;

  // Data for display
  struct BankData
  {
    uint8_t level = 0;
  };
  BankData bankDatas[(uint8_t)KR::BankSize::Quad];

  KSP_MeterQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin()
  {
    Parent::begin(30, false, SWITCH_ADDRESS_METER, OLED12864_ADDRESS, LED24_ADDRESS, EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));

    // Set the Module/Bank draw callbacks
    banks[0].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
    banks[1].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
    banks[2].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
    banks[3].Set(std::bind(&KSP_MeterQuad::onDrawMeterQuad_Level, this, std::placeholders::_1), true);
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
            setBankSelected(encBtn.enc.slider.position);
          }
          else
          if (moduleMode == KR::ModuleMode::Edit)
          {
            setBankLevel(encBtn.enc.slider.position);
          }
        }
      }
    }
  }

  virtual void setEncRange()
  {
      if (moduleMode == KR::ModuleMode::Select)
      {
        encBtn.enc.slider.set(0, getBankSize() - 1, false, bankSelected);
      }
      else
      if (moduleMode == KR::ModuleMode::Edit)
      {
        encBtn.enc.slider.set(0, led24.getSize() - 1, false, bankDatas[bankSelected].level);
      }
  }

  virtual uint8_t setBankLevel(int8_t level)
  {
    resetHighlightTimeout();
    resetModuleModeTimeout();

    bankDatas[bankSelected].level = constrain(level, 0, led24.getSize() - 1);

    return bankDatas[bankSelected].level;
  }

  virtual uint8_t cycleBankLevel(int8_t delta)
  {
    setBankLevel(bankDatas[bankSelected].level + delta);

    return bankDatas[bankSelected].level;
  }

  //------------------------------------------------------------------------------
  // Drawing

  // Meter Quad
  virtual void onDrawMeterQuad_Index(uint8_t index)
  {
    uint8_t bankLevel = bankDatas[index].level;

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
        if (bankLevel > 0)
        {
          led24.setBar(bankLevel - 1, LED_GREEN);
        }
        if (bankLevel < led24.getSize())
        {
          led24.setBar(bankLevel, LED_YELLOW);
        }
        if (bankLevel < led24.getSize() - 1)
        {
          led24.setBar(bankLevel + 1, LED_RED);
        }
      }

      led24.render();
    }
  }

  virtual void onDrawMeterQuad_Time(uint8_t index)
  {
    uint8_t bankLevel = bankDatas[index].level;

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
        if (bankLevel > 0)
        {
          led24.setBar(bankLevel - 1, LED_GREEN);
        }
        if (bankLevel < led24.getSize())
        {
          led24.setBar(bankLevel, LED_YELLOW);
        }
        if (bankLevel < led24.getSize() - 1)
        {
          led24.setBar(bankLevel + 1, LED_RED);
        }
      }

      led24.render();
    }
  }

  virtual void onDrawMeterQuad_Level(uint8_t index)
  {
    uint8_t bankLevel = bankDatas[index].level;

    // OLED
    {
      oled12864.clear();
      drawOledHighlight(index);

      {
        oled12864.gfx.setTextSize(6);
        oled12864.gfx.printf("%02d", bankLevel);
        oled12864.gfx.setTextSize(2);
        oled12864.gfx.printf("#%d\n", index);

        oled12864.gfx.setTextSize(6);
        oled12864.gfx.print("  ");
        oled12864.gfx.setTextSize(2);
        oled12864.gfx.printf("%3d%%\n", (uint8_t)(((float)bankLevel / ((float)led24.getSize() - 1)) * 100));

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
        if (bankLevel > 0)
        {
          led24.setBar(bankLevel - 1, LED_GREEN);
        }
        if (bankLevel < led24.getSize())
        {
          led24.setBar(bankLevel, LED_YELLOW);
        }
        if (bankLevel < led24.getSize() - 1)
        {
          led24.setBar(bankLevel + 1, LED_RED);
        }
      }

      led24.render();
    }
  }
};

#endif// KSP_MeterQuad_h
