// (c) 2025 gogodyne
#ifndef KontrolRack_h
#define KontrolRack_h

#include <Wire.h>
#include "KontrolRack_Timing.h"

namespace KontrolRack {

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

////////////////////////////////////////////////////////////////////////////////
template< size_t N >
class PrintBuffer : public Print
{
  char buffer[ N ];
public:

  ////////////////////////////////////////////////////////////////////////////////
  // Print
  size_t write( uint8_t c )
  {
    int len = strlen( buffer );
    if ( len < sizeof( buffer ) )
      buffer[ len ] = c;

    return ( len < sizeof( buffer ) ) ? 1 : 0;
  }

  ////////////////////////////////////////////////////////////////////////////////
  void cls()              { memset( buffer, 0, sizeof( buffer ) ); }
  void cls( size_t size ) { memset( buffer, 0, min( size, sizeof( buffer ) ) ); }
  char* c_str()           { return buffer; }

  ////////////////////////////////////////////////////////////////////////////////
  int printf( const char* format, ... )
  {
    va_list args;
    int len = strlen( buffer );
    int n = sizeof( buffer ) - len;
    if ( n > 0 )
    {
      va_start( args, format );
      len = vsnprintf( buffer + len, n, format, args );
      va_end( args );
    }

    return len;
  }
};

}// namespace KontrolRack

#endif// KontrolRack_h
