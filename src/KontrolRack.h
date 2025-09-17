// (c) 2025 gogodyne
#ifndef KontrolRack_h
#define KontrolRack_h

#include <Wire.h>
#include "KontrolRack_Timing.h"

namespace KontrolRack {

namespace KR {

enum class BankUnitCount : uint8_t
{
  Mono = 1,
  Dual = 2,
  Quad = 4,
  Octo = 8
};

enum class ModuleMode : int8_t
{
  Normal = 0,
  Select,
  Edit,
};

}// namespace KR

////////////////////////////////////////////////////////////////////////////////
class Utility
{
public:
  static void scanI2c(TwoWire& inWire, Timing* timing = nullptr)
  {
    static timing_t timeout = 0;
    if (timing && (timeout > timing->ms))
    {
      return;
    }

    byte error;
    int deviceCount = 0;

    for (byte address = 1; address < 127; ++address)
    {
      inWire.beginTransmission(address);
      error = inWire.endTransmission();
  
      if (error == 0 || error == 4)
      {
        Serial.print("device: 0x");
        if (address<16)
          Serial.print("0");
        Serial.print(address, HEX);
        Serial.print(" (");
        Serial.print(address);
        Serial.print(")");

        if (error == 0)
        {
          ++deviceCount;
        }
        else
        if (error == 4)
        {
          Serial.print(" unknown error");
        }

        Serial.println();
      }
    }

    Serial.print("I2C devices found: ");
    Serial.println(deviceCount);

    if (timing)
    {
      timeout = timing->ms + 1000;
    }
  }
};

}// namespace KontrolRack

#endif// KontrolRack_h
