// (c) 2025 gogodyne
#ifndef KSP_NumericQuad_h
#define KSP_NumericQuad_h

#include <Preferences.h>
#include <KerbalSimpit.h>
KerbalSimpit mySimpit(Serial);  // Declare a KerbalSimpit object that will communicate using the "Serial" device.
#include <KontrolRack.h>
#include <KontrolRack_KR_Num8OLED12864.h>
#include "KontrolRack_ESPWiFi.h"

using namespace KontrolRack;

#define BANKSCENE_COUNT (1)
#define BANKSCENE_INDEX_KEY "bankSceneIndex"
#define BANKSCENE_ROW_KEYFORMAT "row%d"
#define BANKDATA_VALUESIZE 32
#define BANKSCENE_INDEX_DEFAULT 0

////////////////////////////////////////////////////////////////////////////////
#define s_KSP_NumericQuad "KSP_NumericQuad"// max 15 characters
#define PREFS_Namespace s_KSP_NumericQuad
class KSP_NumericQuad : public KR::Numeric8Quad
{
public:
  using Parent = KR::Numeric8Quad;

  Preferences preferences;
  ESPWiFi net;

  // Bank
  enum class BankDisplayMode : uint8_t
  {
    OFF,

    ApoapsisDistance,
    ApoapsisTime,
    PeriapsisDistance,
    PeriapsisTime,

    SIZE
  };

  struct BankLabel
  {
    const char* label;
    const char* sublabel;
  };
  // Labels per mode
  const BankLabel bankLabels[(int)BankDisplayMode::SIZE] =
  {
    {"--", ""},

    {"Ap", "Dstance to"},
    {"Ap", "Time until"},
    {"Pe", "Dstance to"},
    {"Pe", "Time until"},
  };

  // A preset group of Bank modes; one mode per Bank
  struct BankScene
  {
    BankDisplayMode modes[bankCount];
  };
  BankScene bankScenes[BANKSCENE_COUNT] =
  {
    {{BankDisplayMode::ApoapsisDistance, BankDisplayMode::ApoapsisTime, BankDisplayMode::PeriapsisDistance, BankDisplayMode::PeriapsisTime}},
  };
  uint8_t bankSceneIndex = BANKSCENE_INDEX_DEFAULT;

  // Data for display
  struct DisplayData
  {
    BankDisplayMode bankDisplayMode;
    const char* units = "";
    const char* scale = "";
    char numeric[BANKDATA_VALUESIZE] = {0};

    DisplayData() {}

    DisplayData(BankDisplayMode bankDisplayMode)
    : bankDisplayMode(bankDisplayMode)
    {}

    DisplayData(BankDisplayMode bankDisplayMode, float data)
    : bankDisplayMode(bankDisplayMode)
    {
      units = "m";

      float dataScaled = data;
      float dataAbs = abs(data);

      if (dataAbs >= 1e12)
      {
        scale = "G";
        dataScaled = data / 1e9;
      }
      else
      if (dataAbs >= 1e9)
      {
        scale = "M";
        dataScaled = data / 1e6;
      }
      else
      if (dataAbs >= 1e3)
      {
        scale = "k";
        dataScaled = data / 1e3;
      }

      snprintf(numeric, BANKDATA_VALUESIZE, "%.2f", dataScaled);
    }

    DisplayData(BankDisplayMode bankDisplayMode, int32_t seconds)
    : bankDisplayMode(bankDisplayMode)
    {
      units = "s";
      int32_t s = abs(seconds);

      int32_t d = s / 86400;//24*3600
      s %= 86400;
      int32_t h = s / 3600;//60*60
      s %= 3600;
      int32_t m = s / 60;
      s %= 60;

      snprintf(numeric, BANKDATA_VALUESIZE, "%02ld:%02ld:%02ld:%02ld", d, h, m, s);
    }
  };

  // KSP Messages
  void (*mySimpitHandler)(byte messageType, byte msg[], byte msgSize) = nullptr;
  altitudeMessage altitudeMsg;
  velocityMessage velocityMsg;
  airspeedMessage airspeedMsg;
  apsidesMessage apsidesMsg;
  apsidesTimeMessage apsidesTimeMsg;
  maneuverMessage maneuverMsg;
  SASInfoMessage sasInfoMsg;
  orbitInfoMessage orbitInfoMsg;;
  vesselPointingMessage vesselPointingMsg;

  // Connection.
  int connectionState = 0;
  unsigned long heartbeatNextMs = 0;

  KSP_NumericQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin(void (*messageHandler)(byte messageType, byte msg[], byte msgSize))
  {
    prefsBegin();

    mySimpitHandler = messageHandler;

    // Start the devices
    num8Device.begin(12);

    for (int i = 0; i < getBankCount(); ++i)
    {
      // Start the devices
      oled12864Devices[i].begin(12);
    }

    Parent::begin(12, false, SWITCH_ADDRESS, OLED12864_ADDRESS, Num8::Info(NUM8_DIN, NUM8_CS, NUM8_CLK, NUM8_INTENSITY), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));
    net.begin();
  }

  virtual void loop() override
  {
    Parent::loop();
    net.loop();

    // Input
    switch (bankSelectMode)
    {
    case KR::BankSelectMode::None:
      if (encBtn.didPress())
      {
        setBankSelectMode(KR::BankSelectMode::Select);
      }
      if (encBtn.didChangeEnc())
      {
        cycleBankScene(encBtn.didIncrease());
      }
      break;

    case KR::BankSelectMode::Select:
      if (encBtn.didPress())
      {
        setBankSelectMode(KR::BankSelectMode::Edit);
      }
      if (encBtn.didChangeEnc())
      {
        cycleBankSelected(encBtn.didIncrease());
      }
      break;

    case KR::BankSelectMode::Edit:
      if (encBtn.didPress())
      {
        setBankSelectMode(KR::BankSelectMode::Select);
      }
      if (encBtn.didChangeEnc())
      {
        cycleBankDisplayMode(encBtn.didIncrease());
      }
      break;
    }

    heartbeat();
    mySimpit.update();
  }

  //------------------------------------------------------------------------------

  virtual bool checkBankSelectModeTimeout() override
  {
    if (Parent::checkBankSelectModeTimeout())
    {
      prefsStore();

      return true;
    }

    return false;
  }

  virtual void cycleBankDisplayMode(bool next)
  {
    resetBankSelectModeTimeout();

    BankDisplayMode& bankDisplayMode = bankScenes[bankSceneIndex].modes[bankSelectedIndex];

    if (next)
    {
      if ((int)bankDisplayMode < (int)BankDisplayMode::SIZE - 1)
        bankDisplayMode = (BankDisplayMode)((int)bankDisplayMode + 1);
    }
    else
    {
      if ((int)bankDisplayMode > 0)
        bankDisplayMode = (BankDisplayMode)((int)bankDisplayMode - 1);
    }
  }

  virtual void cycleBankScene(bool next)
  {
    if (next)
    {
      bankSceneIndex = min(bankSceneIndex + 1, BANKSCENE_COUNT - 1);
    }
    else
    {
      bankSceneIndex = max(bankSceneIndex - 1, 0);
    }
  }

  //------------------------------------------------------------------------------
  // Preferences

  virtual void prefsBegin()
  {
    preferences.begin(PREFS_Namespace);
    prefsLoad();
  }

  virtual void prefsStore()
  {
    prefsLoad(false);
  }

  virtual void prefsLoad(bool load = true)
  {
    // Selected Scene index
    {
      const char* key = BANKSCENE_INDEX_KEY;
      uint8_t value = preferences.getUChar(key, BANKSCENE_INDEX_DEFAULT);

      if (load)
      {
        bankSceneIndex = value;
      }
      else
      // store
      {
        if (bankSceneIndex != value)
        {
          preferences.putUChar(key, bankSceneIndex);
        }
      }
    }

    // All Scene rows
    {
      char key[16] = {0};

      for (int i = 0; i < BANKSCENE_COUNT; ++i)
      {
        sprintf(key, BANKSCENE_ROW_KEYFORMAT, i);
        uint8_t modes[bankCount];
        size_t size = 0;

        if (preferences.getType(key) == PT_BLOB)
        {
          size = preferences.getBytes(key, modes, bankCount);
        }

        if (load)
        {
          if (size == bankCount)
          {
            memcpy(bankScenes[i].modes, modes, bankCount);
          }
        }
        else
        // store
        {
          if ((size != bankCount) || memcmp(bankScenes[i].modes, modes, bankCount))
          {
            preferences.putBytes(key, bankScenes[i].modes, bankCount);
          }
        }
      }
    }
  }

  //------------------------------------------------------------------------------
  // Connection

  virtual int heartbeat()
  {
    const int HeartbeatInterval = 5000;
    unsigned long ms = millis();

    // Ping
    if (ms >= heartbeatNextMs)
    {
      heartbeatNextMs = ms + HeartbeatInterval;
      char buf[48] = {0};
      snprintf(buf, 48, "'%s' PING\n", net.hostName);
      // mySimpit.send(ECHO_REQ_MESSAGE, buf, strlen(buf) + 1);
    }

    // Connect
    static unsigned long tryConnectMs = 0;
    if ((connectionState == 0) && (ms >= tryConnectMs))
    {
      tryConnectMs = ms + HeartbeatInterval;
      // connectionState = mySimpit.init();
      if (connectionState)
      {
        onConnect();
      }
    }

    return connectionState;
  }

  virtual void onConnect()
  {
    connectionState += mySimpit.connectedToKSP2();

    mySimpit.inboundHandler(mySimpitHandler);

    // | Vessel Movement/Position |
    mySimpit.registerChannel(ALTITUDE_MESSAGE);
    mySimpit.registerChannel(VELOCITY_MESSAGE);
    mySimpit.registerChannel(AIRSPEED_MESSAGE);
    mySimpit.registerChannel(APSIDES_MESSAGE);
    mySimpit.registerChannel(APSIDESTIME_MESSAGE);
    mySimpit.registerChannel(MANEUVER_MESSAGE);
    mySimpit.registerChannel(SAS_MODE_INFO_MESSAGE);
    mySimpit.registerChannel(ORBIT_MESSAGE);
    mySimpit.registerChannel(ROTATION_DATA_MESSAGE);
  }

  virtual void messageHandler(byte messageType, byte msg[], byte msgSize)
  {
    switch (messageType)
    {
    case ECHO_RESP_MESSAGE:
      {
        connectionState = 1;
        onConnect();
      }
      break;

    case ALTITUDE_MESSAGE:
      {
        if (msgSize == sizeof(altitudeMessage))
        {
          altitudeMsg = parseMessage<altitudeMessage>(msg);
          // myAltitudeSeaLevel = altitudeMsg.sealevel;
          // myAltitudeSurface = altitudeMsg.surface;
        }
      }
      break;

    case VELOCITY_MESSAGE:
      {
        if (msgSize == sizeof(velocityMessage))
        {
          velocityMsg = parseMessage<velocityMessage>(msg);
          // myFloatStorage1 = velocityMsg.orbital;
          // myFloatStorage2 = velocityMsg.surface;
          // myFloatStorage3 = velocityMsg.vertical;
        }
      }
      break;

    case AIRSPEED_MESSAGE:
      {
        if (msgSize == sizeof(airspeedMessage))
        {
          airspeedMsg = parseMessage<airspeedMessage>(msg);
          // myFloatStorage1 = airspeedMsg.IAS;
          // myFloatStorage2 = airspeedMsg.mach;
          // myFloatStorage3 = airspeedMsg.gForces;
        }
      }
      break;

    case APSIDES_MESSAGE:
      {
        if (msgSize == sizeof(apsidesMessage))
        {
          apsidesMsg = parseMessage<apsidesMessage>(msg);
        }
      }
      break;

    case APSIDESTIME_MESSAGE:
      {
        if (msgSize == sizeof(apsidesTimeMessage))
        {
          apsidesTimeMsg = parseMessage<apsidesTimeMessage>(msg);
        }
      }
      break;

    case MANEUVER_MESSAGE:
      {
        if (msgSize == sizeof(maneuverMessage))
        {
          maneuverMsg = parseMessage<maneuverMessage>(msg);
          // myFloatStorage1 = maneuverMsg.timeToNextManeuver;
          // myFloatStorage2 = maneuverMsg.deltaVNextManeuver;
          // myFloatStorage3 = maneuverMsg.durationNextManeuver;
          // myFloatStorage4 = maneuverMsg.deltaVTotal;
          // myFloatStorage5 = maneuverMsg.headingNextManeuver;
          // myFloatStorage6 = maneuverMsg.pitchNextManeuver;
        }
      }
      break;

    case SAS_MODE_INFO_MESSAGE:
      {
        if (msgSize == sizeof(SASInfoMessage))
        {
          sasInfoMsg = parseMessage<SASInfoMessage>(msg);
          // myCurrentSASMode = sasInfoMsg.currentSASMode;
          // mySASModeAvailability = sasInfoMsg.SASModeAvailability;
        }
      }
      break;

    case ORBIT_MESSAGE:
      {
        if (msgSize == sizeof(orbitInfoMessage))
        {
          orbitInfoMsg = parseMessage<orbitInfoMessage>(msg);
          // myFloatStorage1 = orbitInfoMsg.eccentricity;
          // myFloatStorage2 = orbitInfoMsg.semiMajorAxis;
          // myFloatStorage3 = orbitInfoMsg.inclination;
          // myFloatStorage4 = orbitInfoMsg.longAscendingNode;
          // myFloatStorage5 = orbitInfoMsg.argPeriapsis;
          // myFloatStorage6 = orbitInfoMsg.trueAnomaly;
          // myFloatStorage7 = orbitInfoMsg.meanAnomaly;
          // myFloatStorage8 = orbitInfoMsg.period;
        }
      }
      break;

    case ROTATION_DATA_MESSAGE:
      {
        if (msgSize == sizeof(vesselPointingMessage))
        {
          vesselPointingMsg = parseMessage<vesselPointingMessage>(msg);
          // myFloatStorage1 = vesselPointingMsg.heading;
          // myFloatStorage2 = vesselPointingMsg.pitch;
          // myFloatStorage3 = vesselPointingMsg.roll;
          // myFloatStorage4 = vesselPointingMsg.orbitalVelocityHeading;
          // myFloatStorage5 = vesselPointingMsg.orbitalVelocityPitch;
          // myFloatStorage6 = vesselPointingMsg.surfaceVelocityHeading;
          // myFloatStorage7 = vesselPointingMsg.surfaceVelocityPitch;
        }
      }
      break;
    }
  }

  virtual DisplayData makeDisplayData(uint8_t bankIndex)
  {
    BankDisplayMode bankDisplayMode = bankScenes[bankSceneIndex].modes[bankIndex];

    switch (bankDisplayMode)
    {
    default:
    case BankDisplayMode::OFF:
      return DisplayData(bankDisplayMode);

    case BankDisplayMode::ApoapsisDistance:
      return DisplayData(bankDisplayMode, apsidesMsg.apoapsis);

    case BankDisplayMode::ApoapsisTime:
      return DisplayData(bankDisplayMode, apsidesTimeMsg.apoapsis);

    case BankDisplayMode::PeriapsisDistance:
      return DisplayData(bankDisplayMode, apsidesMsg.periapsis);

    case BankDisplayMode::PeriapsisTime:
      return DisplayData(bankDisplayMode, apsidesTimeMsg.periapsis);
    }

    return DisplayData();
  }

  //------------------------------------------------------------------------------
  // Drawing

  virtual bool isDrawBankTick(uint8_t bankIndex) override
  {
    return oled12864Devices[bankIndex].timing.isTick || num8Device.timing.isTick;
  }

  virtual void drawBank(uint8_t bankIndex, bool isDirty) override
  {
    Parent::drawBank(bankIndex, isDirty);

    const int LABELSIZE = 4;
    const int LABELSIZE_SM = 2;
    DisplayData displayData = makeDisplayData(bankIndex);
    BankLabel bankLabel = bankLabels[(int)displayData.bankDisplayMode];

    // OLED
    if (isDirty || oled12864Devices[bankIndex].timing.isTick)
    {
      oled12864.clear();
      {
        if (bankLabel.label)
        {
          int16_t x1, y1;
          uint16_t w, h;

          // Label size
          oled12864.gfx.setTextSize(LABELSIZE);
          oled12864.gfx.getTextBounds(bankLabel.label, 0, 0, &x1, &y1, &w, &h);

          // Label frame
          uint16_t labelHeight = LABELSIZE + h + LABELSIZE;
          oled12864.gfx.drawRoundRect(1, 1, oled12864.gfx.width() - 2, labelHeight - 2, LABELSIZE + LABELSIZE, WHITE);

          // Scale and Units
          oled12864.gfx.setTextSize(LABELSIZE);
          oled12864.gfx.setCursor(LABELSIZE, LABELSIZE);
          oled12864.gfx.print(displayData.scale);
          oled12864.gfx.print(displayData.units);

          oled12864.gfx.setTextSize(LABELSIZE_SM);
          oled12864.gfx.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          {
            // Bank Scene index
            oled12864.gfx.printf("%X", bankSceneIndex);

            // Connection status
            if (connectionState < 1)
            {
              oled12864.gfx.print((connectionState < 0) ? "*" : timing.isHz(2) ? "/" : "\\");
            }
          }
          oled12864.gfx.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

          // Label
          oled12864.gfx.setTextSize(LABELSIZE);
          oled12864.gfx.setCursor(oled12864.gfx.width() - w - LABELSIZE, LABELSIZE);
          oled12864.gfx.printf(bankLabel.label);

          // Sublabel
          oled12864.gfx.setTextSize(LABELSIZE_SM);
          oled12864.gfx.setCursor(LABELSIZE, labelHeight + LABELSIZE);
          oled12864.gfx.printf(bankLabel.sublabel);
        }
      }
      drawOledEffects(bankIndex);
      oled12864.render();
    }

    // Numeric LED
    if (isDirty || num8Device.timing.isTick)
    {
      // (Numeric is not I2C; Clear is done in the draw)

      num8.printBank(bankIndex, displayData.numeric);

      drawNum8Effects(bankIndex);
      // (Numeric is not I2C; Render is done in the draw)
    }
  }
};

#endif// KSP_NumericQuad_h
