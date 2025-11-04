// (c) 2025 gogodyne
#ifndef KSP_NumericQuad_h
#define KSP_NumericQuad_h

#include <Preferences.h>
#include <nvs_flash.h>
#include <KerbalSimpit.h>
KerbalSimpit mySimpit(Serial);  // Declare a KerbalSimpit object that will communicate using the "Serial" device.
#include <KontrolRack.h>
#include <KontrolRack_KR_Num8OLED12864.h>
#include "KSP_NumericQuadWeb.h"

// Use this to CLEAR *ALL* NVS and freeze
// #define PREFS_CLEAR

using namespace KontrolRack;

#define BANKSCENE_COUNT (16)
#define BANKSCENE_INDEX_KEY "bankSceneIndex"
#define BANKSCENE_ROW_KEYFORMAT "row%d"

#define BANKDATA_VALUESIZE 32

namespace UnitNames
{
  const char* none = "";
  const char* meters = "m";
  const char* metersPerSecond = "m/s";
  const char* seconds = "s";
  const char* degrees = "deg";
  const char* G = "G";
  const char* Kelvin = "K";
  const char* Mach = "M";
  const char* Newtons = "N";
}//namespace UnitNames

////////////////////////////////////////////////////////////////////////////////
#define s_KSP_NumericQuad "KSP_NumericQuad"// max 15 characters
#define PREFS_Namespace s_KSP_NumericQuad
class KSP_NumericQuad : public KR::Numeric8Quad
{
public:
  using Parent = KR::Numeric8Quad;

  //------------------------------------------------------------------------------
  struct Menu
  {
    enum struct Mode { Off, Cfg };
    enum struct Cfg { Done, Connect, Web, SIZE };

    Mode mode = Mode::Off;
    Cfg cfg = Cfg::Done;

    bool isOff() const
    {
      return mode == Mode::Off;
    }

    bool isCfg() const
    {
      return mode == Mode::Cfg;
    }

    bool isCfg(Cfg other) const
    {
      return isCfg() && cfg == other;
    }

    void setOff()
    {
      mode = Mode::Off;
    }

    void setCfg(Cfg other)
    {
      mode = Mode::Cfg; cfg = other;
    }
  };
  Menu menu;

  // Bank
  enum class BankDisplayMode : uint8_t
  {
    // Do Not Reorder; used for settings
    OFF,

    // | Vessel Movement/Position |

    AltitudeSeaLevel,
    AltitudeSurface,

    VelocityOrbital,
    VelocitySurface,
    VelocityVertical,

    AirspeedIAS,
    AirspeedMach,
    AirspeedGForces,

    ApoapsisDistance,
    ApoapsisTime,
    PeriapsisDistance,
    PeriapsisTime,

    ManeuverTimeToNext,
    ManeuverDeltaVNext,
    ManeuverDurationNext,
    ManeuverDeltaVTotal,
    ManeuverHeadingNext,
    ManeuverPitchNext,

    OrbitEccentricity,
    OrbitSemiMajorAxis,
    OrbitInclination,
    OrbitLongAscendingNode,
    OrbitArgPeriapsis,
    OrbitTrueAnomaly,
    OrbitMeanAnomaly,
    OrbitPeriod,

    OrientationHeading,
    OrientationPitch,
    OrientationRoll,
    OrientationOrbitalVelocityHeading,
    OrientationOrbitalVelocityPitch,
    OrientationSurfaceVelocityHeading,
    OrientationSurfaceVelocityPitch,

    // | External Environment |

    TargetDistance,
    TargetVelocity,
    TargetHeading,
    TargetPitch,
    TargetVelocityHeading,
    TargetVelocityPitch,

    AtmosphereAirDensity,
    AtmosphereTemperature,
    AtmospherePressure,

    IntersectsDistanceAtIntersect1,
    IntersectsVelocityAtIntersect1,
    IntersectsTimeToIntersect1,
    IntersectsDistanceAtIntersect2,
    IntersectsVelocityAtIntersect2,
    IntersectsTimeToIntersect2,

    SIZE
  };

  struct BankLabel
  {
    const char* name;
    const char* label;
    const char* sublabel;
  };

  // Labels per mode
  const BankLabel bankLabels[(int)BankDisplayMode::SIZE] =
  {
    {"(off)",               "", ""},

    // | Vessel Movement/Position |

    {"Altitude (Sea)",      "Alt",  "Sea Level" },
    {"Altitude (Surf)",     "Alt",  "Surface"   },

    {"Velocity (Orb)",      "Vel",  "Orbital"   },
    {"Velocity (Surf)",     "Vel",  "Surface"   },
    {"Velocity (Vert)",     "Vel",  "Vertical"  },

    {"Air Speed",           "AIS",  "Air Speed" },
    {"Mach Speed",          "Mch",  "Mach"      },
    {"G-Forces",            "G-F",  "G-Forces"  },

    {"Apoapsis Distance",   "Apo",  "Dstance to"},
    {"Apoapsis Time",       "Apo",  "Time until"},
    {"Periapsis Distance",  "Per",  "Dstance to"},
    {"Periapsis Time",      "Per",  "Time until"},

    {"Maneuver Time Until", "Mnv",  "Time until"},
    {"Maneuver dV Next",    "Mnv",  "dV Next"   },
    {"Maneuver Duration",   "Mnv",  "Duration"  },
    {"Maneuver dV All",     "Mnv",  "dV All"    },
    {"Maneuver Heading",    "Mnv",  "Heading"   },
    {"Maneuver Pitch",      "Mnv",  "Pitch"     },

    {"Orbit Eccentricity",  "Orb",  "Eccentrcty"},
    {"Orbit SemiMajorAxis", "Orb",  "SemiMajor" },
    {"Orbit Inclination",   "Orb",  "Inclnation"},
    {"Orbit Long Ascend",   "Orb",  "Long Ascnd"},
    {"Orbit Arg Periapsis", "Orb",  "Arg Peri"  },
    {"Orbit True Anomaly",  "Orb",  "True Anom" },
    {"Orbit Mean Anomaly",  "Orb",  "Mean Anom" },
    {"Orbit Period",        "Orb",  "Period"    },

    {"Vessel Heading",      "Ori",  "Heading"   },
    {"Vessel Pitch",        "Ori",  "Pitch"     },
    {"Vessel Roll",         "Ori",  "Roll"      },
    {"Orbital Vel Heading", "Ori",  "Orb Headng"},
    {"Orbital Vel Pitch",   "Ori",  "Orb Pitch" },
    {"Surface Vel Heading", "Ori",  "Surf Headg"},
    {"Surface Vel Pitch",   "Ori",  "Surf Pitch"},

    // | External Environment |

    {"Target Distance",     "Trg",  "Targ Dist" },
    {"Target Velocity",     "Trg",  "Targ Vel"  },
    {"Target Heading",      "Trg",  "Targ Headg"},
    {"Target Pitch",        "Trg",  "Targ Pitch"},
    {"Target Vel Heading",  "Trg",  "Trg VelHdg"},
    {"Target Vel Pitch",    "Trg",  "Trg VelPch"},

    {"Atmosphere Density",  "Atm",  "Atm Dens"  },
    {"Atmosphere Temp",     "Atm",  "Atm Temp"  },
    {"Atmosphere Pressure", "Atm",  "Atm Prss"  },

    {"Intersect1 Distance", "Int",  "Int 1 Dist"},
    {"Intersect1 Velocity", "Int",  "Int 1 Vel" },
    {"Intersect1 Time To",  "Int",  "Int 1 Time"},
    {"Intersect2 Distance", "Int",  "Int 2 Dist"},
    {"Intersect2 Velocity", "Int",  "Int 2 Vel" },
    {"Intersect2 Time To",  "Int",  "Int 2 Time"},
  };

  // A preset group of Bank modes; one mode per Bank
  struct BankScene
  {
    BankDisplayMode modes[bankCount];
  };
  BankScene bankScenes[BANKSCENE_COUNT] =
  {
    {{BankDisplayMode::ApoapsisDistance, BankDisplayMode::ApoapsisTime, BankDisplayMode::PeriapsisDistance, BankDisplayMode::PeriapsisTime}},
    {{BankDisplayMode::AltitudeSeaLevel, BankDisplayMode::AltitudeSurface, BankDisplayMode::VelocityOrbital, BankDisplayMode::VelocitySurface}},
    {{BankDisplayMode::VelocityVertical, BankDisplayMode::AirspeedIAS, BankDisplayMode::AirspeedMach, BankDisplayMode::AirspeedGForces}},
    {{BankDisplayMode::ManeuverTimeToNext, BankDisplayMode::ManeuverDeltaVNext, BankDisplayMode::ManeuverDurationNext, BankDisplayMode::ManeuverDeltaVTotal}},

    {{BankDisplayMode::OrbitEccentricity, BankDisplayMode::OrbitSemiMajorAxis, BankDisplayMode::OrbitInclination, BankDisplayMode::OrbitLongAscendingNode}},
    {{BankDisplayMode::OrbitArgPeriapsis, BankDisplayMode::OrbitTrueAnomaly, BankDisplayMode::OrbitMeanAnomaly, BankDisplayMode::OrbitPeriod}},
    {{BankDisplayMode::OrientationHeading, BankDisplayMode::OrientationPitch, BankDisplayMode::OrientationRoll, BankDisplayMode::OFF}},
    {{BankDisplayMode::OrientationOrbitalVelocityHeading, BankDisplayMode::OrientationOrbitalVelocityPitch, BankDisplayMode::OrientationSurfaceVelocityHeading, BankDisplayMode::OrientationSurfaceVelocityPitch}},

    {{BankDisplayMode::TargetDistance, BankDisplayMode::TargetVelocity, BankDisplayMode::TargetHeading, BankDisplayMode::TargetPitch}},
    {{BankDisplayMode::TargetDistance, BankDisplayMode::TargetVelocity, BankDisplayMode::TargetVelocityHeading, BankDisplayMode::TargetVelocityPitch}},
    {{BankDisplayMode::AtmosphereAirDensity, BankDisplayMode::AtmosphereTemperature, BankDisplayMode::AtmospherePressure, BankDisplayMode::OFF}},
    {{BankDisplayMode::IntersectsDistanceAtIntersect1, BankDisplayMode::IntersectsVelocityAtIntersect1, BankDisplayMode::IntersectsTimeToIntersect1, BankDisplayMode::OFF}},

    {{BankDisplayMode::IntersectsDistanceAtIntersect2, BankDisplayMode::IntersectsVelocityAtIntersect2, BankDisplayMode::IntersectsTimeToIntersect2, BankDisplayMode::OFF}},
    {{BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF}},
    {{BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF}},
    {{BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF}},
  };
  uint8_t bankSceneIndex = 0;

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

    DisplayData(BankDisplayMode bankDisplayMode, float data, const char* unitsIn)
    : bankDisplayMode(bankDisplayMode)
    {
      units = unitsIn;

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
      units = UnitNames::seconds;
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

  Preferences preferences;

  // KSP Messages
  void (*mySimpitHandler)(byte messageType, byte msg[], byte msgSize) = nullptr;
  // | Vessel Movement/Position |
  altitudeMessage altitudeMsg;
  velocityMessage velocityMsg;
  airspeedMessage airspeedMsg;
  apsidesMessage apsidesMsg;
  apsidesTimeMessage apsidesTimeMsg;
  maneuverMessage maneuverMsg;
  orbitInfoMessage orbitInfoMsg;;
  vesselPointingMessage vesselPointingMsg;
  // | External Environment |
  targetMessage targetMsg;
  atmoConditionsMessage atmoConditionsMsg;
  intersectsMessage intersectsMsg;

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
  unsigned long heartbeatNextMs = 0;

  // Long-press
  timing_t longPressMs = 0;
  bool isLongPress = false;

  // Web
  KSP_NumericQuadWeb web;

  KSP_NumericQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin(void (*messageHandler)(byte messageType, byte msg[], byte msgSize))
  {
    prefsBegin();
    web.begin();

    mySimpitHandler = messageHandler;

    // Start the devices
    num8Device.begin(NUM8_FPS);

    for (int i = 0; i < getBankCount(); ++i)
    {
      // Start the devices
      oled12864Devices[i].begin(OLED12864_FPS);
    }

    Parent::begin(MODULE_FPS, false, SWITCH_ADDRESS, OLED12864_ADDRESS, Num8::Info(NUM8_DIN, NUM8_CS, NUM8_CLK, NUM8_INTENSITY), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));
  }

  virtual void loop() override
  {
    Parent::loop();
    web.loop();

    if (menu.isOff())
    {
      inputBanks();

      heartbeat();
    }
    else
    {
      inputMenu();
    }

    mySimpit.update();
  }

  //------------------------------------------------------------------------------

  virtual void inputBanks()
  {
    switch (bankSelectMode)
    {
    case KR::BankSelectMode::None:
      if (encBtn.didRelease() && !isLongPress)
      {
        setBankSelectMode(KR::BankSelectMode::Select);
      }
      if (encBtn.didChangeEnc())
      {
        cycleBankScene(encBtn.didIncrease());
      }
      break;

    case KR::BankSelectMode::Select:
      if (encBtn.didRelease())
      {
        setBankSelectMode(KR::BankSelectMode::Edit);
      }
      if (encBtn.didChangeEnc())
      {
        cycleBankSelected(encBtn.didIncrease());
      }
      break;

    case KR::BankSelectMode::Edit:
      if (encBtn.didRelease())
      {
        setBankSelectMode(KR::BankSelectMode::Select);
      }
      if (encBtn.didChangeEnc())
      {
        cycleBankDisplayMode(encBtn.didIncrease());
      }
      break;
    }

    // Long button press
    bool wasLongPress = isLongPress;
    if (encBtn.isPress())
    {
      if (longPressMs == 0)
      {
        longPressMs = timing.ms;
      }
      else
      {
        isLongPress = (timing.ms - longPressMs) > 3000;
      }
    }
    else
    {
      longPressMs = 0;
      isLongPress = false;
    }

    // Menu
    if (menu.isOff())
    {
      if (isLongPress && !wasLongPress)
      {
        menu.setCfg(Menu::Cfg::Done);
      }
    }
  }

  virtual void inputMenu()
  {
    if (!menu.isOff())
    {
      if (encBtn.didPress())
      {
        if (menu.isCfg(Menu::Cfg::Done))
        {
          // In case of issues...
          num8.reset();

          web.net.WiFi_Disconnect(true);
          menu.setOff();
        }

        if (menu.isCfg(Menu::Cfg::Connect))
        {
          kspStatus.toggleConnecting();
        }

        if (menu.isCfg(Menu::Cfg::Web))
        {
          // toggle
          if (web.net.WiFi_IsConnected())
          {
            web.net.WiFi_Disconnect(true);
          }
          else
            // not connected
            if (!web.net.WiFi_IsBusy())
            {
              web.net.WiFi_ConnectAP();
            }
        }
      }

      if (encBtn.didChangeEnc())
      {
        if (menu.isCfg())
        {
          int selection = (int)menu.cfg;
          selection += encBtn.didIncrease() ? +1 : -1;
          menu.cfg = (Menu::Cfg)constrain(selection, 0, (int)Menu::Cfg::SIZE - 1);
        }
      }
    }
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
      uint8_t value = preferences.getUChar(key, 0);

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

  virtual void heartbeat()
  {
    const int HeartbeatInterval = 5000;

    // Ping
    if (timing.ms >= heartbeatNextMs)
    {
      heartbeatNextMs = timing.ms + HeartbeatInterval;

      mySimpit.send(ECHO_REQ_MESSAGE, web.net.hostName, strlen(web.net.hostName) + 1);
    }

    // Connect
    if (kspStatus.isConnecting())
    {
      if (mySimpit.init())
      {
        onConnect();
      }
    }
  }

  virtual void onConnect()
  {
    kspStatus.connect(mySimpit.connectedToKSP2());

    mySimpit.inboundHandler(mySimpitHandler);

    // | Vessel Movement/Position |
    mySimpit.registerChannel(ALTITUDE_MESSAGE);
    mySimpit.registerChannel(VELOCITY_MESSAGE);
    mySimpit.registerChannel(AIRSPEED_MESSAGE);
    mySimpit.registerChannel(APSIDES_MESSAGE);
    mySimpit.registerChannel(APSIDESTIME_MESSAGE);
    mySimpit.registerChannel(MANEUVER_MESSAGE);
    mySimpit.registerChannel(ORBIT_MESSAGE);
    mySimpit.registerChannel(ROTATION_DATA_MESSAGE);
    // | External Environment |
    mySimpit.registerChannel(TARGETINFO_MESSAGE);
    mySimpit.registerChannel(ATMO_CONDITIONS_MESSAGE);
    mySimpit.registerChannel(INTERSECTS_MESSAGE);
  }

  virtual void messageHandler(byte messageType, byte msg[], byte msgSize)
  {
    switch (messageType)
    {
    case ECHO_RESP_MESSAGE:
      {
        if (!kspStatus.isConnected())
        {
          onConnect();
        }
      }
      break;

    // | Vessel Movement/Position |

    case ALTITUDE_MESSAGE:
      {
        if (msgSize == sizeof(altitudeMessage))
        {
          altitudeMsg = parseMessage<altitudeMessage>(msg);
        }
      }
      break;

    case VELOCITY_MESSAGE:
      {
        if (msgSize == sizeof(velocityMessage))
        {
          velocityMsg = parseMessage<velocityMessage>(msg);
        }
      }
      break;

    case AIRSPEED_MESSAGE:
      {
        if (msgSize == sizeof(airspeedMessage))
        {
          airspeedMsg = parseMessage<airspeedMessage>(msg);
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
        }
      }
      break;

    case ORBIT_MESSAGE:
      {
        if (msgSize == sizeof(orbitInfoMessage))
        {
          orbitInfoMsg = parseMessage<orbitInfoMessage>(msg);
        }
      }
      break;

    case ROTATION_DATA_MESSAGE:
      {
        if (msgSize == sizeof(vesselPointingMessage))
        {
          vesselPointingMsg = parseMessage<vesselPointingMessage>(msg);
        }
      }
      break;

    // | External Environment |

    case TARGETINFO_MESSAGE:
      {
        if (msgSize == sizeof(targetMessage)) 
        {
          targetMsg = parseMessage<targetMessage>(msg);
        }
      } break;

    case ATMO_CONDITIONS_MESSAGE:
      {
        if (msgSize == sizeof(atmoConditionsMessage)) 
        {
          atmoConditionsMsg = parseMessage<atmoConditionsMessage>(msg);
        }
      } break;

    case INTERSECTS_MESSAGE:
      {
        if (msgSize == sizeof(intersectsMessage)) 
        {
          intersectsMsg = parseMessage<intersectsMessage>(msg);
        }
      }
      break;
    }
  }

  //------------------------------------------------------------------------------
  // Drawing

  virtual DisplayData makeDisplayData(uint8_t bankIndex)
  {
    BankDisplayMode bankDisplayMode = bankScenes[bankSceneIndex].modes[bankIndex];

    switch (bankDisplayMode)
    {
    default:
    case BankDisplayMode::OFF:
      return DisplayData(bankDisplayMode);

    // | Vessel Movement/Position |

    case BankDisplayMode::AltitudeSeaLevel:
      return DisplayData(bankDisplayMode, altitudeMsg.sealevel, UnitNames::meters);

    case BankDisplayMode::AltitudeSurface:
      return DisplayData(bankDisplayMode, altitudeMsg.surface, UnitNames::meters);


    case BankDisplayMode::VelocityOrbital:
      return DisplayData(bankDisplayMode, velocityMsg.orbital, UnitNames::metersPerSecond);

    case BankDisplayMode::VelocitySurface:
      return DisplayData(bankDisplayMode, velocityMsg.surface, UnitNames::metersPerSecond);

    case BankDisplayMode::VelocityVertical:
      return DisplayData(bankDisplayMode, velocityMsg.vertical, UnitNames::metersPerSecond);


    case BankDisplayMode::AirspeedIAS:
      return DisplayData(bankDisplayMode, airspeedMsg.IAS, UnitNames::metersPerSecond);

    case BankDisplayMode::AirspeedMach:
      return DisplayData(bankDisplayMode, airspeedMsg.mach, UnitNames::Mach);

    case BankDisplayMode::AirspeedGForces:
      return DisplayData(bankDisplayMode, airspeedMsg.gForces, UnitNames::G);


    case BankDisplayMode::ApoapsisDistance:
      return DisplayData(bankDisplayMode, apsidesMsg.apoapsis, UnitNames::meters);

    case BankDisplayMode::ApoapsisTime:
      return DisplayData(bankDisplayMode, apsidesTimeMsg.apoapsis);

    case BankDisplayMode::PeriapsisDistance:
      return DisplayData(bankDisplayMode, apsidesMsg.periapsis, UnitNames::meters);

    case BankDisplayMode::PeriapsisTime:
      return DisplayData(bankDisplayMode, apsidesTimeMsg.periapsis);


    case BankDisplayMode::ManeuverTimeToNext:
      return DisplayData(bankDisplayMode, maneuverMsg.timeToNextManeuver);

    case BankDisplayMode::ManeuverDeltaVNext:
      return DisplayData(bankDisplayMode, maneuverMsg.deltaVNextManeuver, UnitNames::metersPerSecond);

    case BankDisplayMode::ManeuverDurationNext:
      return DisplayData(bankDisplayMode, maneuverMsg.durationNextManeuver);

    case BankDisplayMode::ManeuverDeltaVTotal:
      return DisplayData(bankDisplayMode, maneuverMsg.deltaVTotal, UnitNames::metersPerSecond);

    case BankDisplayMode::ManeuverHeadingNext:
      return DisplayData(bankDisplayMode, maneuverMsg.headingNextManeuver, UnitNames::degrees);

    case BankDisplayMode::ManeuverPitchNext:
      return DisplayData(bankDisplayMode, maneuverMsg.pitchNextManeuver, UnitNames::degrees);


    case BankDisplayMode::OrbitEccentricity:
      return DisplayData(bankDisplayMode, orbitInfoMsg.eccentricity, UnitNames::none);

    case BankDisplayMode::OrbitSemiMajorAxis:
      return DisplayData(bankDisplayMode, orbitInfoMsg.semiMajorAxis, UnitNames::meters);

    case BankDisplayMode::OrbitInclination:
      return DisplayData(bankDisplayMode, orbitInfoMsg.inclination, UnitNames::degrees);

    case BankDisplayMode::OrbitLongAscendingNode:
      return DisplayData(bankDisplayMode, orbitInfoMsg.longAscendingNode, UnitNames::meters);

    case BankDisplayMode::OrbitArgPeriapsis:
      return DisplayData(bankDisplayMode, orbitInfoMsg.argPeriapsis, UnitNames::degrees);

    case BankDisplayMode::OrbitTrueAnomaly:
      return DisplayData(bankDisplayMode, orbitInfoMsg.trueAnomaly, UnitNames::degrees);

    case BankDisplayMode::OrbitMeanAnomaly:
      return DisplayData(bankDisplayMode, orbitInfoMsg.meanAnomaly, UnitNames::degrees);

    case BankDisplayMode::OrbitPeriod:
      return DisplayData(bankDisplayMode, orbitInfoMsg.period);


    case BankDisplayMode::OrientationHeading:
      return DisplayData(bankDisplayMode, vesselPointingMsg.heading, UnitNames::degrees);

    case BankDisplayMode::OrientationPitch:
      return DisplayData(bankDisplayMode, vesselPointingMsg.pitch, UnitNames::degrees);

    case BankDisplayMode::OrientationRoll:
      return DisplayData(bankDisplayMode, vesselPointingMsg.roll, UnitNames::degrees);

    case BankDisplayMode::OrientationOrbitalVelocityHeading:
      return DisplayData(bankDisplayMode, vesselPointingMsg.orbitalVelocityHeading, UnitNames::degrees);

    case BankDisplayMode::OrientationOrbitalVelocityPitch:
      return DisplayData(bankDisplayMode, vesselPointingMsg.orbitalVelocityPitch, UnitNames::degrees);

    case BankDisplayMode::OrientationSurfaceVelocityHeading:
      return DisplayData(bankDisplayMode, vesselPointingMsg.surfaceVelocityHeading, UnitNames::degrees);

    case BankDisplayMode::OrientationSurfaceVelocityPitch:
      return DisplayData(bankDisplayMode, vesselPointingMsg.surfaceVelocityPitch, UnitNames::degrees);

    // | External Environment |

    case BankDisplayMode::TargetDistance:
      return DisplayData(bankDisplayMode, targetMsg.distance, UnitNames::meters);

    case BankDisplayMode::TargetVelocity:
      return DisplayData(bankDisplayMode, targetMsg.velocity, UnitNames::metersPerSecond);

    case BankDisplayMode::TargetHeading:
      return DisplayData(bankDisplayMode, targetMsg.heading, UnitNames::degrees);

    case BankDisplayMode::TargetPitch:
      return DisplayData(bankDisplayMode, targetMsg.pitch, UnitNames::degrees);

    case BankDisplayMode::TargetVelocityHeading:
      return DisplayData(bankDisplayMode, targetMsg.velocityHeading, UnitNames::degrees);

    case BankDisplayMode::TargetVelocityPitch:
      return DisplayData(bankDisplayMode, targetMsg.velocityPitch, UnitNames::degrees);


    case BankDisplayMode::AtmosphereAirDensity:
      return DisplayData(bankDisplayMode, atmoConditionsMsg.airDensity, UnitNames::none);

    case BankDisplayMode::AtmosphereTemperature:
      return DisplayData(bankDisplayMode, atmoConditionsMsg.temperature, UnitNames::Kelvin);

    case BankDisplayMode::AtmospherePressure:
      return DisplayData(bankDisplayMode, atmoConditionsMsg.pressure, UnitNames::Newtons);


    case BankDisplayMode::IntersectsDistanceAtIntersect1:
      return DisplayData(bankDisplayMode, intersectsMsg.distanceAtIntersect1, UnitNames::meters);

    case BankDisplayMode::IntersectsVelocityAtIntersect1:
      return DisplayData(bankDisplayMode, intersectsMsg.velocityAtIntersect1, UnitNames::metersPerSecond);

    case BankDisplayMode::IntersectsTimeToIntersect1:
      return DisplayData(bankDisplayMode, intersectsMsg.timeToIntersect1);

    case BankDisplayMode::IntersectsDistanceAtIntersect2:
      return DisplayData(bankDisplayMode, intersectsMsg.distanceAtIntersect2, UnitNames::meters);

    case BankDisplayMode::IntersectsVelocityAtIntersect2:
      return DisplayData(bankDisplayMode, intersectsMsg.velocityAtIntersect2, UnitNames::metersPerSecond);

    case BankDisplayMode::IntersectsTimeToIntersect2:
      return DisplayData(bankDisplayMode, intersectsMsg.timeToIntersect2);
    }

    return DisplayData();
  }

  virtual void drawMenu(uint8_t bankIndex)
  {
    if (menu.isCfg())
    {
      if (bankIndex == 0)
      {
        oled12864.gfx.setTextSize(SSD1306::Size::Xs, SSD1306::Size::Sm);
        oled12864.gfx.setCursor(0, 0);

        drawMenuItem("Done", menu.isCfg(Menu::Cfg::Done));
        drawMenuItem((kspStatus.isConnecting() ? "Stop Connectng to KSP" : "Try Connectng to KSP"), menu.isCfg(Menu::Cfg::Connect));
        drawMenuItem((web.net.WiFi_IsConnected() ? "Stop Web Config" : "Start Web Config"), menu.isCfg(Menu::Cfg::Web));

        if (web.net.WiFi_IsBusy())
        {
          oled12864.gfx.println(timing.isHz(2) ? "/" : "\\");
        }

        if (web.net.WiFi_IsConnected())
        {
          if (WiFi.softAPgetStationNum() > 0)
          {
            oled12864.gfx.println(WiFi.softAPIP());
          }
          else
          {
            oled12864.gfx.print(web.net.hostName);
          }
        }
      }

      if (bankIndex == 1)
      {
        if (web.net.WiFi_IsConnected())
        {
          oled12864.gfx.setCursor(0, 0);
          oled12864.gfx.setTextSize(SSD1306::Size::Sm);
          oled12864.gfx.println("step");
          oled12864.gfx.setTextSize(SSD1306::Size::X2);

          // None connected to the AP; show the SSID
          if (WiFi.softAPgetStationNum() == 0)
          {
            oled12864.gfx.print("1");
            web.drawQRCode_SSID(oled12864.gfx, oled12864.gfx.width() / 2, 0);
          }
          else
            // A client is connected; show the URL
          {
            oled12864.gfx.print("2");
            web.drawQRCode_URL(oled12864.gfx, oled12864.gfx.width() / 2, 0);
          }
        }
      }
    }
  }

  virtual void drawMenuItem(const char* itemLabel, bool isSelected, bool newline = true)
  {
    bool highlight = isSelected && timing.isHz(2);

    if (highlight)
    {
      oled12864.gfx.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    }
    oled12864.gfx.print(itemLabel);
    if (newline)
    {
      oled12864.gfx.println();
    }
    if (highlight)
    {
      oled12864.gfx.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }
  }

  virtual bool isDrawBankTick(uint8_t bankIndex) override
  {
    return oled12864Devices[bankIndex].timing.isTick || num8Device.timing.isTick;
  }

  virtual void drawBank(uint8_t bankIndex, bool isDirty) override
  {
    Parent::drawBank(bankIndex, isDirty);

    DisplayData displayData = makeDisplayData(bankIndex);
    BankLabel bankLabel = bankLabels[(int)displayData.bankDisplayMode];

    // OLED
    if (isDirty || oled12864Devices[bankIndex].timing.isTick)
    {
      oled12864.clear();
      {
        if (!menu.isOff())
        {
          drawMenu(bankIndex);
        }
        else
        if (bankLabel.label)
        {
          int16_t x1, y1;
          uint16_t w, h;
          uint16_t margin = SSD1306::Size::Lg;

          // Label size
          oled12864.gfx.setTextSize(SSD1306::Size::Lg);
          oled12864.gfx.getTextBounds("X", 0, 0, &x1, &y1, &w, &h);
          uint16_t labelHeight = margin + h + margin;

          // Label frame
          oled12864.gfx.drawRoundRect(1, 1, oled12864.gfx.width() - 2, labelHeight - 2, margin + margin, WHITE);

          // Scale and Units
          oled12864.gfx.setTextSize(((strlen(displayData.scale) + strlen(displayData.units)) > 2) ? SSD1306::Size::Md : SSD1306::Size::Lg);
          oled12864.gfx.setCursor(margin, margin);
          oled12864.gfx.print(displayData.scale);
          oled12864.gfx.print(displayData.units);

          // Info
          if ((bankIndex == 0) || (bankIndex == 1))
          {
            oled12864.gfx.setTextSize(SSD1306::Size::Sm);
            oled12864.gfx.getTextBounds("X", 0, 0, &x1, &y1, &w, &h);
            oled12864.gfx.setCursor((oled12864.gfx.width() - w) / 2, labelHeight - h + margin);
            oled12864.gfx.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            {
              if (bankIndex == 0)
              {
                // Bank Scene index
                oled12864.gfx.printf("%X", bankSceneIndex);

                // Connection status
                if (!kspStatus.isConnected())
                {
                  oled12864.gfx.print(kspStatus.isConnecting() ? (timing.isHz(2) ? "/" : "\\") : "!");
                }
              }
            }
            oled12864.gfx.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
          }

          // Label
          oled12864.gfx.setTextSize((strlen(bankLabel.label) > 2) ? SSD1306::Size::Md : SSD1306::Size::Lg);
          oled12864.gfx.getTextBounds(bankLabel.label, 0, 0, &x1, &y1, &w, &h);
          oled12864.gfx.setCursor(oled12864.gfx.width() - w - margin, margin);
          oled12864.gfx.printf(bankLabel.label);

          oled12864.gfx.setCursor(margin, labelHeight + margin);
          if (bankSelectMode == KR::BankSelectMode::None)
          {
            // Sublabel
            oled12864.gfx.setTextSize(SSD1306::Size::Sm);
            oled12864.gfx.printf(bankLabel.sublabel);
          }
          else
          // Editing
          {
            oled12864.gfx.setTextSize(SSD1306::Size::Xs);
            // Bank Mode name
            oled12864.gfx.print(bankLabel.name);
          }
        }
      }
      drawOledEffects(bankIndex);
      oled12864.render();
    }

    // Numeric LED
    if (isDirty || num8Device.timing.isTick)
    {
      // (Numeric is not I2C; Clear is done in the draw)
      {
        num8.printBank(bankIndex, displayData.numeric);
      }
      drawNum8Effects(bankIndex);
      // (Numeric is not I2C; Render is done in the draw)
    }
  }
};

#endif// KSP_NumericQuad_h
