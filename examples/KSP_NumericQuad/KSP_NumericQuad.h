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

  enum BankMode
  {
    Off,
    Time,
    Level,
  };

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
    // Start the devices.
    num8Device.begin(12);

    for (int i = 0; i < getBankSize(); ++i)
    {
      banks[i].mode = BankMode::Time;

      // Start the devices.
      oled12864Devices[i].begin(12);
    }

    Parent::begin(12, true, SWITCH_ADDRESS, OLED12864_ADDRESS, Num8::Info(NUM8_DIN, NUM8_CS, NUM8_CLK, NUM8_INTENSITY), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));
  }

  virtual void loop() override
  {
    Parent::loop();

    // Input...
    {
      // Encoder click
      if (encBtn.btn.didPress)
      {
        // set mode
        cycleBankSelectMode();
        setEncRange();
      }

      // Encoder changed
      if (encBtn.encDelta)
      {
        // change to select/edit?
        if (bankSelectMode == KR::BankSelectMode::Normal)
        {
          // set mode
          setBankSelectMode(KR::BankSelectMode::Select);
          setEncRange();
        }
        else
        {
          if (bankSelectMode == KR::BankSelectMode::Select)
          {
            setBankSelected(encBtn.enc.slider.position);
          }
          else
          if (bankSelectMode == KR::BankSelectMode::Edit)
          {
            setBankLevel(encBtn.enc.slider.position);
          }
        }
      }
    }
  }

  virtual void setEncRange()
  {
    if (bankSelectMode == KR::BankSelectMode::Select)
    {
      encBtn.enc.slider.set(0, getBankSize() - 1, false, bankSelected);
    }
    else
    if (bankSelectMode == KR::BankSelectMode::Edit)
    {
      encBtn.enc.slider.set(0, LEVEL_RANGE - 1, false, bankDatas[bankSelected].level);
    }
  }

  virtual uint8_t setBankLevel(int8_t level)
  {
    resetHighlightTimeout();
    resetBankSelectModeTimeout();

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

  virtual void drawBank(uint8_t index, bool isDirty)
  {
    Parent::drawBank(index, isDirty);

    // OLED
    if (isDirty || oled12864Devices[index].timing.isTick)
    {
      oled12864.clear();

      switch (banks[index].mode)
      {
      case BankMode::Time:
        {
          oled12864.gfx.setTextSize(4);
          oled12864.gfx.println("ms");
          oled12864.gfx.setTextSize(2);
          oled12864.gfx.printf("%03.1f", timing.fpsEstimate);
        }
        break;

      case BankMode::Level:
        {
          oled12864.gfx.setTextSize(4);
          oled12864.gfx.printf("%d\n", bankDatas[index].level);
          oled12864.gfx.setTextSize(2);
          oled12864.gfx.printf("%03.1f", timing.fpsEstimate);
        }
        break;
      }

      drawOledHighlight(index);
      oled12864.render();
    }

    // Numeric LED
    if (isDirty || num8Device.timing.isTick)
    {
      // (Numeric is not I2C; Clear is done in the draw)

      switch (banks[index].mode)
      {
      case BankMode::Time:
        {
          num8.printBank(index, timing.ms);
        }
        break;

      case BankMode::Level:
        {
          num8.printBank(index, bankDatas[index].level);
        }
        break;
      }

      drawNumHighlight(index);
      // (Numeric is not I2C; Render is done in the draw)
    }
  }
};

#endif// KSP_NumericQuad_h
