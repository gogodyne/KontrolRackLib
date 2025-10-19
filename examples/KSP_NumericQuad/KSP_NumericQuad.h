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
    // Start the devices.
    num8Device.begin(12);

    for (int i = 0; i < getBankSize(); ++i)
    {
      // Set the Bank draw callbacks.
      setBankToDraw_Time(i);

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

  virtual void setBankToDraw_Time(uint8_t bankIndex)
  {
    banks[bankIndex].setDrawCallback(
      [this](uint8_t index, bool isDirty)
      {
        // OLED
        if (oled12864Devices[index].timing.isTick)
        {
          oled12864.clear();

          oled12864.gfx.setTextSize(4);
          oled12864.gfx.println("ms");
          oled12864.gfx.setTextSize(2);
          oled12864.gfx.printf("%03.1f", timing.fpsEstimate);

          drawOledHighlight(index);
          oled12864.render();
        }

        // Numeric LED
        if (num8Device.timing.isTick)
        {
          // Numeric is not I2C; clear is done in the draw

          num8.printBank(index, timing.ms);

          drawNumHighlight(index);
          // Numeric is not I2C; render is done in the draw
        }
      }
      );
  }
  
  virtual void setBankToDraw_Level(uint8_t bankIndex)
  {
    banks[bankIndex].setDrawCallback(
      [this](uint8_t index, bool isDirty)
      {
        // OLED
        if (oled12864Devices[index].timing.isTick)
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
        if (num8Device.timing.isTick)
        {
          // Numeric is not I2C; clear is done in the draw

          num8.printBank(index, bankDatas[index].level);

          drawNumHighlight(index);
          // Numeric is not I2C; render is done in the draw
        }
      }
      );
  }
};

#endif// KSP_NumericQuad_h
