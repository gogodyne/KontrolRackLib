// (c) 2025 gogodyne
#ifndef KSP_NumericQuad_h
#define KSP_NumericQuad_h

#include <KontrolRack.h>
#include <KontrolRack_KR_Num8OLED12864.h>

using namespace KontrolRack;

////////////////////////////////////////////////////////////////////////////////
class KSP_NumericQuad : public KR::Numeric8Quad
{
public:
  using Parent = KR::Numeric8Quad;

  // Data for display
  struct BankData
  {
    uint8_t level = 0;
  };
  BankData bankDatas[(uint8_t)KR::BankSize::Quad];

  KSP_NumericQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin()
  {
    Parent::begin(30, false, SWITCH_ADDRESS, OLED12864_ADDRESS, Num8::Info(NUM8_DIN, NUM8_CS, NUM8_CLK, NUM8_INTENSITY), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));

    // Set the Module/Bank draw callbacks
    banks[0].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
    banks[1].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
    banks[2].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
    banks[3].Set(std::bind(&KSP_NumericQuad::onDrawNumericQuad_Level, this, std::placeholders::_1), true);
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
        cycleBankMode();
        setEncRange();
      }

      // Encoder changed
      if (encBtn.encDelta)
      {
        // change to select/edit?
        if (bankMode == KR::BankMode::Normal)
        {
          // set mode
          setBankMode(KR::BankMode::Select);
          setEncRange();
        }
        else
        {
          if (bankMode == KR::BankMode::Select)
          {
            setBankSelected(encBtn.enc.slider.position);
          }
          else
          if (bankMode == KR::BankMode::Edit)
          {
            setBankLevel(encBtn.enc.slider.position);
          }
        }
      }
    }
  }

  virtual void setEncRange()
  {
      if (bankMode == KR::BankMode::Select)
      {
        encBtn.enc.slider.set(0, getBankSize() - 1, false, bankSelected);
      }
      else
      if (bankMode == KR::BankMode::Edit)
      {
        encBtn.enc.slider.set(0, LEVEL_RANGE - 1, false, bankDatas[bankSelected].level);
      }
  }

  virtual uint8_t setBankLevel(int8_t level)
  {
    resetHighlightTimeout();
    resetBankModeTimeout();

    bankDatas[bankSelected].level = constrain(level, 0, LEVEL_RANGE - 1);

    return bankDatas[bankSelected].level;
  }

  virtual uint8_t cycleBankLevel(int8_t delta)
  {
    setBankLevel(bankDatas[bankSelected].level + delta);

    return bankDatas[bankSelected].level;
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

      num8.printBank(index, timing.ms);

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
      oled12864.gfx.printf("%d\n", bankDatas[index].level);
      oled12864.gfx.setTextSize(2);
      oled12864.gfx.printf("%03.1f", timing.fpsEstimate);

      drawOledHighlight(index);
      oled12864.render();
    }

    // Numeric LED
    {
      // Numeric is not I2C; clear is done in the draw

      num8.printBank(index, bankDatas[index].level);

      drawNumHighlight(index);
      // Numeric is not I2C; render is done in the draw
    }
  }
};

#endif// KSP_NumericQuad_h
