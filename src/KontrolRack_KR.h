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

typedef std::function<void(uint8_t index)> DrawUnitCallback;

////////////////////////////////////////////////////////////////////////////////
// A "Unit" is a group of one or more devices intended to be one of an array of identical Units.
struct UnitInfo
{
  // set to draw the next frame
  bool isDirty = true;
  // set to reset the Dirty flag after drawing; it will clear otherwise
  bool autoDirty = true;
  // set to perform drawing for this unit; no drawing if null
  DrawUnitCallback onDrawUnit = nullptr;

  virtual void Set(DrawUnitCallback onDrawUnit, bool autoDirty)
  {
    this->onDrawUnit = onDrawUnit;
    this->autoDirty = autoDirty;
  }

  virtual void DrawUnit(uint8_t index)
  {
    if (onDrawUnit && isDirty) onDrawUnit(index);
    isDirty = autoDirty;
  }
};

////////////////////////////////////////////////////////////////////////////////
// A "Module" is a group of one or more devices or Units.
class Module
{
public:
  Timing timing;
  TwoWire& wire;
  I2cSwitch i2cSwitch;
  EncBtn encBtn;
  bool useEncBtn = false;

  DrawUnitCallback onDrawUnit;

  UnitInfo* _unitInfos = nullptr;
  int8_t unitSelected = 0;
  ModuleMode moduleMode = ModuleMode::Normal;
  timing_t moduleModeTimeout = 0;
  // keeps the highlight solid while the selection is changing
  timing_t highlightTimeout = 0;

  Module(TwoWire& inWire)
  : wire(inWire)
  {}

  // This defines the size of the bank
  virtual uint8_t getUnitCount() const = 0;

  // to disable enc/btn: positionCount < 1
  virtual void begin(uint8_t switchAddress, EncBtn::Info encInfo, fps_t fps, bool test)
  {
    timing.begin(fps);
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
    if (moduleModeTimeout < timing.ms)
    {
      setModuleMode(ModuleMode::Normal);
    }
  }

  // Draw all bank units.
  virtual void draw()
  {
    for (int i = 0; i < getUnitCount(); ++i)
    {
      drawUnit(i);
    }
  }

  // Draw a bank unit.
  virtual void drawUnit(uint8_t index)
  {
    openUnitPorts(index);

    // callback
    // if (onDrawUnit) onDrawUnit(index);
    if (_unitInfos) _unitInfos[index].DrawUnit(index);
  }

  // Extend the highlight emphasis.
  virtual void resetHighlightTimeout()
  {
    highlightTimeout = timing.ms + 500;
  }

  // Select a bank unit.
  virtual int8_t setUnitSelected(int8_t index)
  {
    resetHighlightTimeout();
    resetModuleModeTimeout();
    unitSelected = constrain(index, 0, getUnitCount() - 1);

    return unitSelected;
  }

  // Select next/prev bank unit.
  virtual int8_t cycleUnitSelected(int delta)
  {
    setUnitSelected(unitSelected + delta);

    return unitSelected;
  }

  // Extend the mode timeout.
  virtual void resetModuleModeTimeout()
  {
    moduleModeTimeout = timing.ms + 3000;
  }

  // Set the module mode.
  virtual void setModuleMode(ModuleMode inModuleMode)
  {
    resetModuleModeTimeout();
    moduleMode = inModuleMode;
  }

  // Set the next/prev module mode.
  virtual ModuleMode cycleModuleMode(bool next = true)
  {
    switch (moduleMode)
    {
    case ModuleMode::Normal:
      setModuleMode(next ? ModuleMode::Select : ModuleMode::Edit);
      break;
    case ModuleMode::Select:
      setModuleMode(next ? ModuleMode::Edit : ModuleMode::Normal);
      break;
    case ModuleMode::Edit:
      setModuleMode(next ? ModuleMode::Normal : ModuleMode::Select);
      break;
    }

    return moduleMode;
  }

  // Open the Switch ports for a bank unit [0-3].
  // (0-3) also opens (4-7); up to 4 units of 2 devices each, on 8-channel MUX.
  virtual void openUnitPorts(uint8_t index)
  {
    index = index & 0b11;
    uint8_t portBits = (bit(index) << 4) | bit(index);
    i2cSwitch.setPortBits(portBits);
  }
};

}// namespace KR

}// namespace KontrolRack

#endif// KontrolRack_KR_h
