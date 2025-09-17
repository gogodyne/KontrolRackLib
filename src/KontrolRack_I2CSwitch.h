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
    driver.setPort(portNumber);
  }

  // Set all ports.
  virtual void setPortBits(uint8_t portBits)
  {
    driver.setPortState(portBits);
  }

  // Get bit position of the first enabled port.
  virtual uint8_t getPort()
  {
    return driver.getPort();
  }

  // Get all port states.
  virtual uint8_t getPortState()
  {
    return driver.getPortState();
  }

  // Enable individual port.
  virtual bool enablePort(uint8_t portNumber)
  {
    return driver.enablePort(portNumber);
  }

  // Disable individual port.
  virtual bool disablePort(uint8_t portNumber)
  {
    return driver.disablePort(portNumber);
  }
};

}// namespace KontrolRack

#endif// KontrolRack_I2cSwitch_h