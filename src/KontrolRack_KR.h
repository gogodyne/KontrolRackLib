// (c) 2025 gogodyne
#ifndef KontrolRack_KR_h
#define KontrolRack_KR_h

#include <Wire.h>
#include "KontrolRack.h"
#include "KontrolRack_Timing.h"
#include "KontrolRack_Input.h"
#include "KontrolRack_I2cSwitch.h"

namespace KontrolRack {

namespace KR {

enum class BankSize : uint8_t
{
  Mono = 1,
  Dual = 2,
  Quad = 4
};

enum class BankSelectMode : int8_t
{
  Normal = 0,
  Select,
  Edit,
};

////////////////////////////////////////////////////////////////////////////////
// A group of one or more devices intended to be part of an array of identical groups.
struct Bank
{
  // An output or display device that needs a throttled update rate.
  // Used for each device in a row.
  struct Device
  {
    Timing timing;

    virtual void begin(timing_t fps)
    {
      timing.begin(fps);
    }

    virtual void loop()
    {
      timing.loop();
    }
  };

  int mode = 0;
};

////////////////////////////////////////////////////////////////////////////////
// A group of one or more devices.
// This class layer handles device looping; child classes should override.
class Module
{
public:
  Timing timing;

  Module()
  {}

  virtual void begin(fps_t fps, bool test)
  {
    timing.begin(fps);
  }

  virtual void loop()
  {
    loopDevices();
  }

  virtual void loopDevices()
  {}
};

////////////////////////////////////////////////////////////////////////////////
// A Module that supports I2C devices through an I2C switch.
// This class layer supports a rotary encoder with a button.
class ModuleI2C : public Module
{
public:
  using Parent = Module;

  // I2C
  TwoWire& wire;
  I2cSwitch i2cSwitch;

  // Encoder/button
  EncBtn encBtn;
  bool useEncBtn = false;

  // Banks
  Bank* _banks = nullptr;
  int8_t bankSelected = 0;
  BankSelectMode bankSelectMode = BankSelectMode::Normal;
  timing_t bankSelectModeTimeout = 0;
  // keeps the highlight solid while the selection is changing
  timing_t highlightTimeout = 0;

  ModuleI2C(TwoWire& inWire)
  : Parent()
  , wire(inWire)
  {}

  // This returns the number of banks
  virtual uint8_t getBankCount() const = 0;

  using Parent::begin;
  // to disable enc/btn: positionCount < 1
  virtual void begin(fps_t fps, bool test, uint8_t switchAddress, EncBtn::Info encInfo)
  {
    Parent::begin(fps, test);

    // Init encoder.
    if (encInfo.positionCount > 0)
    {
      encBtn.begin(encInfo);
      useEncBtn = true;
    }

    // Init I2C switch.
    i2cSwitch.begin(switchAddress, wire);
    i2cSwitch.setPortBits(0);
  }

  virtual void loop()
  {
    Parent::loop();

    timing.loop();
    if (useEncBtn)
    {
      encBtn.loop();
    }

    bool isDirty = encBtn.didChange();

    // Draw the Module.
    if (timing.isTick || isDirty)
    {
      draw();
    }

    // Draw all Banks.
    if (i2cSwitch.isConnected())
    {
      drawBanks(isDirty);
    }

    // Automatic mode timeout.
    if (bankSelectModeTimeout < timing.ms)
    {
      setBankSelectMode(BankSelectMode::Normal);
    }
  }

  // Draw the Module.
  virtual void draw()
  {
  }

  // Override to draw each device of a Bank.
  virtual void drawBank(uint8_t bankIndex, bool isDirty)
  {
  }

  // Override to flag drawing for devices.
  virtual bool isDrawBankTick(uint8_t bankIndex)
  {
    return false;
  }

  virtual void drawBanks(bool isDirty)
  {
    for (int bankIndex = 0; bankIndex < getBankCount(); ++bankIndex)
    {
      if (isDrawBankTick(bankIndex))
      {
        openBankPorts(bankIndex);
        drawBank(bankIndex, isDirty);
      }
    }
  }

  // Extend the highlight emphasis.
  virtual void resetHighlightTimeout()
  {
    highlightTimeout = timing.ms + 500;
  }

  // Select a bank.
  virtual int8_t setBankSelected(int8_t bankIndex)
  {
    resetHighlightTimeout();
    resetBankSelectModeTimeout();
    bankSelected = constrain(bankIndex, 0, getBankCount() - 1);

    return bankSelected;
  }

  // Select next/prev bank.
  virtual int8_t cycleBankSelected(int delta)
  {
    setBankSelected(bankSelected + delta);

    return bankSelected;
  }

  // Extend the mode timeout.
  virtual void resetBankSelectModeTimeout()
  {
    bankSelectModeTimeout = timing.ms + 3000;
  }

  // Set the module mode.
  virtual void setBankSelectMode(BankSelectMode inBankSelectMode)
  {
    resetBankSelectModeTimeout();
    bankSelectMode = inBankSelectMode;
  }

  // Set the next/prev module mode.
  virtual BankSelectMode cycleBankSelectMode(bool next = true)
  {
    switch (bankSelectMode)
    {
    case BankSelectMode::Normal:
      setBankSelectMode(next ? BankSelectMode::Select : BankSelectMode::Edit);
      break;
    case BankSelectMode::Select:
      setBankSelectMode(next ? BankSelectMode::Edit : BankSelectMode::Normal);
      break;
    case BankSelectMode::Edit:
      setBankSelectMode(next ? BankSelectMode::Normal : BankSelectMode::Select);
      break;
    }

    return bankSelectMode;
  }

  // Open the Switch ports for a bank [0-3].
  // (0-3) also opens (4-7); up to 4 banks of 2 devices each, on 8-channel MUX.
  virtual void openBankPorts(uint8_t bankIndex)
  {
    bankIndex = bankIndex & 0b11;
    uint8_t portBits = (bit(bankIndex) << 4) | bit(bankIndex);
    i2cSwitch.setPortBits(portBits);
  }
};

}// namespace KR

}// namespace KontrolRack

#endif// KontrolRack_KR_h
