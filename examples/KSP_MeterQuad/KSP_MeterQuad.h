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

  enum BankMode
  {
    Off,
    Index,
    Time,
    Level,
  };

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
    for (int i = 0; i < getBankSize(); ++i)
    {
      banks[i].mode = BankMode::Level;

      // Start the devices.
      led24Devices[i].begin(12);
      oled12864Devices[i].begin(12);
    }

    Parent::begin(12, false, SWITCH_ADDRESS_METER, OLED12864_ADDRESS, LED24_ADDRESS, EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));
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
      encBtn.enc.slider.set(0, led24.getSize() - 1, false, bankDatas[bankSelected].level);
    }
  }

  virtual uint8_t setBankLevel(int8_t level)
  {
    resetHighlightTimeout();
    resetBankSelectModeTimeout();

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

  virtual void drawBank(uint8_t index, bool isDirty) override
  {
    Parent::drawBank(index, isDirty);

    uint8_t bankLevel = bankDatas[index].level;

    // OLED
    if (isDirty || oled12864Devices[index].timing.isTick)
    {
      oled12864.clear();
      drawOledHighlight(index);

      switch (banks[index].mode)
      {
      case BankMode::Index:
        {
          oled12864.gfx.setTextSize(2);
          oled12864.gfx.printf("#%d\n", index);
        }
        break;

      case BankMode::Time:
        {
          oled12864.gfx.printf("\n%d", (int)timing.ms);
        }
        break;

      case BankMode::Level:
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
        break;
      }

      oled12864.render();
    }

    // LED24
    if (isDirty || led24Devices[index].timing.isTick)
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
