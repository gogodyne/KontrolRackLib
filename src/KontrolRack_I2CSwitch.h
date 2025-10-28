// (c) 2025 gogodyne
#ifndef KontrolRack_I2cSwitch_h
#define KontrolRack_I2cSwitch_h

#include <SparkFun_I2C_Mux_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_I2C_Mux

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
class I2cSwitch
{
public:
  QWIICMUX driver;

  virtual void begin(uint8_t i2cAddress = QWIIC_MUX_DEFAULT_ADDRESS, TwoWire& withWire = Wire)
  {
    driver.begin(i2cAddress, withWire);
  }

  virtual bool isConnected()
  {
    return driver.isConnected();
  }

  // Open one port only.
  virtual void setPort(uint8_t portNumber)
  {
    if (isConnected())
    driver.setPort(portNumber);
  }

  // Set all ports.
  virtual void setPortBits(uint8_t portBits)
  {
    if (isConnected())
    driver.setPortState(portBits);
  }

  // Get bit position of the first enabled port.
  virtual uint8_t getPort()
  {
    if (isConnected())
    return driver.getPort();
    return 0;
  }

  // Get all port states.
  virtual uint8_t getPortState()
  {
    if (isConnected())
    return driver.getPortState();
    return 0;
  }

  // Enable individual port.
  virtual bool enablePort(uint8_t portNumber)
  {
    if (isConnected())
    return driver.enablePort(portNumber);
    return false;
  }

  // Disable individual port.
  virtual bool disablePort(uint8_t portNumber)
  {
    if (isConnected())
    return driver.disablePort(portNumber);
    return false;
  }
};

}// namespace KontrolRack

#endif// KontrolRack_I2cSwitch_h