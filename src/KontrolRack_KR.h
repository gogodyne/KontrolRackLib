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

enum class BankMode : int8_t
{
  Normal = 0,
  Select,
  Edit,
};

typedef std::function<void(uint8_t index)> DrawBankCallback;

////////////////////////////////////////////////////////////////////////////////
// A group of one or more devices intended to be part of an array of identical groups.
struct Bank
{
  // set to draw the next frame
  bool isDirty = true;
  // set to reset the Dirty flag after drawing; it will clear otherwise
  bool autoDirty = true;
  // set to perform drawing for this bank; no drawing if null
  DrawBankCallback onDrawBank = nullptr;

  virtual void Set(DrawBankCallback onDrawBank, bool autoDirty)
  {
    this->onDrawBank = onDrawBank;
    this->autoDirty = autoDirty;
  }

  virtual void DrawBank(uint8_t index)
  {
    if (onDrawBank && isDirty) onDrawBank(index);
    isDirty = autoDirty;
  }
};

////////////////////////////////////////////////////////////////////////////////
// A group of one or more devices.
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
};

////////////////////////////////////////////////////////////////////////////////
// A Module that supports I2C devices.
// Also supports a rotary encoder with a button.
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

  DrawBankCallback onDrawBank;

  Bank* _banks = nullptr;
  int8_t bankSelected = 0;
  BankMode bankMode = BankMode::Normal;
  timing_t bankModeTimeout = 0;
  // keeps the highlight solid while the selection is changing
  timing_t highlightTimeout = 0;

  ModuleI2C(TwoWire& inWire)
  : Parent()
  , wire(inWire)
  {}

  // This defines the size of the bank
  virtual uint8_t getBankSize() const = 0;

  using Parent::begin;
  // to disable enc/btn: positionCount < 1
  virtual void begin(fps_t fps, bool test, uint8_t switchAddress, EncBtn::Info encInfo)
  {
    Parent::begin(fps, test);

    if (encInfo.positionCount > 0)
    {
      encBtn.begin(encInfo);
      useEncBtn = true;
    }

    // init I2C switch
    i2cSwitch.begin(switchAddress, wire);
    i2cSwitch.setPortBits(0);
  }

  virtual void loop(bool dirtyByEncoder = true)
  {
    timing.loop();
    if (useEncBtn)
    {
      encBtn.loop();
    }

    bool isDirty = (dirtyByEncoder && (encBtn.btn.didChange || encBtn.encDelta));
    if (timing.isTick || isDirty)
    {
      draw();
    }

    // automatic mode timeout
    if (bankModeTimeout < timing.ms)
    {
      setBankMode(BankMode::Normal);
    }
  }

  // Draw all banks.
  virtual void draw()
  {
    for (int i = 0; i < getBankSize(); ++i)
    {
      drawBank(i);
    }
  }

  // Draw a bank.
  virtual void drawBank(uint8_t index)
  {
    openBankPorts(index);

    // callback
    // if (onDrawBank) onDrawBank(index);
    if (_banks) _banks[index].DrawBank(index);
  }

  // Extend the highlight emphasis.
  virtual void resetHighlightTimeout()
  {
    highlightTimeout = timing.ms + 500;
  }

  // Select a bank.
  virtual int8_t setBankSelected(int8_t index)
  {
    resetHighlightTimeout();
    resetBankModeTimeout();
    bankSelected = constrain(index, 0, getBankSize() - 1);

    return bankSelected;
  }

  // Select next/prev bank.
  virtual int8_t cycleBankSelected(int delta)
  {
    setBankSelected(bankSelected + delta);

    return bankSelected;
  }

  // Extend the mode timeout.
  virtual void resetBankModeTimeout()
  {
    bankModeTimeout = timing.ms + 3000;
  }

  // Set the module mode.
  virtual void setBankMode(BankMode inBankMode)
  {
    resetBankModeTimeout();
    bankMode = inBankMode;
  }

  // Set the next/prev module mode.
  virtual BankMode cycleBankMode(bool next = true)
  {
    switch (bankMode)
    {
    case BankMode::Normal:
      setBankMode(next ? BankMode::Select : BankMode::Edit);
      break;
    case BankMode::Select:
      setBankMode(next ? BankMode::Edit : BankMode::Normal);
      break;
    case BankMode::Edit:
      setBankMode(next ? BankMode::Normal : BankMode::Select);
      break;
    }

    return bankMode;
  }

  // Open the Switch ports for a bank [0-3].
  // (0-3) also opens (4-7); up to 4 banks of 2 devices each, on 8-channel MUX.
  virtual void openBankPorts(uint8_t index)
  {
    index = index & 0b11;
    uint8_t portBits = (bit(index) << 4) | bit(index);
    i2cSwitch.setPortBits(portBits);
  }
};

}// namespace KR

}// namespace KontrolRack

#endif// KontrolRack_KR_h
