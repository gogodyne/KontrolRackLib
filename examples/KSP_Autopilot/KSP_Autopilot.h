// (c) 2025 gogodyne
#ifndef KSP_Autopilot_h
#define KSP_Autopilot_h

#include <KerbalSimpit.h>
KerbalSimpit mySimpit(Serial);  // Declare a KerbalSimpit object that will communicate using the "Serial" device.
#include <KontrolRack_LEDButton.h>

using namespace KontrolRack;

////////////////////////////////////////////////////////////////////////////////
class KSP_Autopilot : public LEDButton10
{
public:
  using Parent = LEDButton10;

  // Presets
  byte modes[10] =
  {
    AutopilotMode::AP_PROGRADE,
    AutopilotMode::AP_RETROGRADE,
    AutopilotMode::AP_NORMAL,
    AutopilotMode::AP_ANTINORMAL,
    AutopilotMode::AP_RADIALIN,
    AutopilotMode::AP_RADIALOUT,
    AutopilotMode::AP_TARGET,
    AutopilotMode::AP_ANTITARGET,
    AutopilotMode::AP_STABILITYASSIST,
    AutopilotMode::AP_MANEUVER,
    // AutopilotMode::AP_NAVIGATION,
    // AutopilotMode::AP_AUTOPILOT,
  };

  // KSP Messages
  void (*mySimpitHandler)(byte messageType, byte msg[], byte msgSize) = nullptr;
  // | Vessel Movement/Position |
  SASInfoMessage SASInfoMsg;

  // KSP connection
  struct KSPStatus
  {
    enum State
    {
      Disconnected = -1,
      Connecting = 0,
      ConnectedKSP = 1,
      ConnectedKSP2 = 2,
    };
    int state = State::Connecting;

    virtual void connect(bool isKSP2)
    {
      state = isKSP2 ? State::ConnectedKSP2 : State::ConnectedKSP;
    }

    virtual bool isConnected() const
    {
      return state > 0;
    }

    virtual bool isConnectedKSP2() const
    {
      return state == State::ConnectedKSP2;
    }

    virtual bool isConnecting() const
    {
      return state == State::Connecting;
    }

    virtual void toggleConnecting()
    {
      state = isConnecting() ? State::Disconnected : State::Connecting;
    }
  };
  KSPStatus kspStatus;

  using Parent::begin;
  virtual void begin(void (*messageHandler)(byte messageType, byte msg[], byte msgSize), uint8_t btn8_pinIn, uint8_t btn8_pinOut, uint8_t btn9_pinIn, uint8_t btn9_pinOut, uint8_t i2c_addr = PCF8575_I2CADDR_DEFAULT, TwoWire *wire = &Wire)
  {
    mySimpitHandler = messageHandler;

    for (uint8_t i = 0; i < 10; ++i)
    {
      LEDButton& btn = getButton(i);
      btn.setOutputMode(LEDButton::OutputMode::Active);
    }

    Parent::begin(btn8_pinIn, btn8_pinOut, btn9_pinIn, btn9_pinOut, i2c_addr, wire);
  }

  virtual void loop()
  {
    Parent::loop();

    checkConnection();

    mySimpit.update();
  }

  //------------------------------------------------------------------------------
  // Input

  virtual void updateAutopilot()
  {
    for (uint8_t i = 0; i < 10; ++i)
    {
      LEDButton& btn = getButton(i);
      int outputMode = LEDButton::OutputMode::Off;
      if ((1 << modes[i]) & SASInfoMsg.SASModeAvailability)
      {
        outputMode = (SASInfoMsg.currentSASMode == modes[i]) ? LEDButton::OutputMode::Active : LEDButton::OutputMode::On;
      }
      btn.setOutputMode(outputMode);
    }
  }

  //------------------------------------------------------------------------------
  // Connection

  virtual void checkConnection()
  {
    // Trying to connect
    if (kspStatus.isConnecting())
    {
      // Try and did connect
      if (mySimpit.init())
      {
        kspStatus.connect(mySimpit.connectedToKSP2());

        mySimpit.inboundHandler(mySimpitHandler);

        // | Vessel Movement/Position |
        mySimpit.registerChannel(SAS_MODE_INFO_MESSAGE);
      }
    }
  }

  virtual void messageHandler(byte messageType, byte msg[], byte msgSize)
  {
    switch (messageType)
    {
    case ECHO_RESP_MESSAGE:
      {
      }
      break;

    // | Vessel Movement/Position |

    case SAS_MODE_INFO_MESSAGE:
      {
        if (msgSize == sizeof(SASInfoMessage))
        {
          SASInfoMsg = parseMessage<SASInfoMessage>(msg);
          updateAutopilot();
        }
      }
      break;
    }
  }
};

#endif// KSP_MeterQuad_h
