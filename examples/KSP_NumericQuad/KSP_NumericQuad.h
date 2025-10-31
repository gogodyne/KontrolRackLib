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

    VesselPointingHeading,
    VesselPointingPitch,
    VesselPointingRoll,
    VesselPointingOrbitalVelocityHeading,
    VesselPointingOrbitalVelocityPitch,
    VesselPointingSurfaceVelocityHeading,
    VesselPointingSurfaceVelocityPitch,

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
    IntersectsIimeToIntersect2,

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
    {"(off)",               "--", ""},

    // | Vessel Movement/Position |

    {"Altitude (Sea)",      "Alt",  "Sea Level" },
    {"Altitude (Surf)",     "Alt",  "Surface"   },

    {"Velocity (Orb)",      "Vel",  "Orbital"   },
    {"Velocity (Surf)",     "Vel",  "Surface"   },
    {"Velocity (Vert)",     "Vel",  "Vertical"  },

    {"Air Speed",           "AIS",  "Air Speed" },
    {"Mach Speed",          "Mch",  "Mach"      },
    {"G-Forces",            "GF",   "G-Forces"  },

    {"Apoapsis Distance",   "Ap",   "Dstance to"},
    {"Apoapsis Time",       "Ap",   "Time until"},
    {"Periapsis Distance",  "Pe",   "Dstance to"},
    {"Periapsis Time",      "Pe",   "Time until"},

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
    {"Orbit True Anomaly",  "Orb",  "Long Ascnd"},
    {"Orbit Mean Anomaly",  "Orb",  "Long Ascnd"},
    {"Orbit Period",        "Orb",  "Long Ascnd"},

    {"Vessel Heading",      "Poi",  "Heading"   },
    {"Vessel Pitch",        "Poi",  "Pitch"     },
    {"Vessel Roll",         "Poi",  "Roll"      },
    {"Orbital Vel Heading", "Poi",  "Orb Headng"},
    {"Orbital Vel Pitch",   "Poi",  "Orb Pitch" },
    {"Surface Vel Heading", "Poi",  "Surf Headg"},
    {"Surface Vel Pitch",   "Poi",  "Surf Pitch"},

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
        connectionState = 1;
        onConnect();
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
      return DisplayData(bankDisplayMode, altitudeMsg.sealevel);

    case BankDisplayMode::AltitudeSurface:
      return DisplayData(bankDisplayMode, altitudeMsg.surface);


    case BankDisplayMode::VelocityOrbital:
      return DisplayData(bankDisplayMode, velocityMsg.orbital);

    case BankDisplayMode::VelocitySurface:
      return DisplayData(bankDisplayMode, velocityMsg.surface);

    case BankDisplayMode::VelocityVertical:
      return DisplayData(bankDisplayMode, velocityMsg.vertical);


    case BankDisplayMode::AirspeedIAS:
      return DisplayData(bankDisplayMode, airspeedMsg.IAS);

    case BankDisplayMode::AirspeedMach:
      return DisplayData(bankDisplayMode, airspeedMsg.mach);

    case BankDisplayMode::AirspeedGForces:
      return DisplayData(bankDisplayMode, airspeedMsg.gForces);


    case BankDisplayMode::ApoapsisDistance:
      return DisplayData(bankDisplayMode, apsidesMsg.apoapsis);

    case BankDisplayMode::ApoapsisTime:
      return DisplayData(bankDisplayMode, apsidesTimeMsg.apoapsis);

    case BankDisplayMode::PeriapsisDistance:
      return DisplayData(bankDisplayMode, apsidesMsg.periapsis);

    case BankDisplayMode::PeriapsisTime:
      return DisplayData(bankDisplayMode, apsidesTimeMsg.periapsis);


    case BankDisplayMode::ManeuverTimeToNext:
      return DisplayData(bankDisplayMode, maneuverMsg.timeToNextManeuver);

    case BankDisplayMode::ManeuverDeltaVNext:
      return DisplayData(bankDisplayMode, maneuverMsg.deltaVNextManeuver);

    case BankDisplayMode::ManeuverDurationNext:
      return DisplayData(bankDisplayMode, maneuverMsg.durationNextManeuver);

    case BankDisplayMode::ManeuverDeltaVTotal:
      return DisplayData(bankDisplayMode, maneuverMsg.deltaVTotal);

    case BankDisplayMode::ManeuverHeadingNext:
      return DisplayData(bankDisplayMode, maneuverMsg.headingNextManeuver);

    case BankDisplayMode::ManeuverPitchNext:
      return DisplayData(bankDisplayMode, maneuverMsg.pitchNextManeuver);


    case BankDisplayMode::OrbitEccentricity:
      return DisplayData(bankDisplayMode, orbitInfoMsg.eccentricity);

    case BankDisplayMode::OrbitSemiMajorAxis:
      return DisplayData(bankDisplayMode, orbitInfoMsg.semiMajorAxis);

    case BankDisplayMode::OrbitInclination:
      return DisplayData(bankDisplayMode, orbitInfoMsg.inclination);

    case BankDisplayMode::OrbitLongAscendingNode:
      return DisplayData(bankDisplayMode, orbitInfoMsg.longAscendingNode);

    case BankDisplayMode::OrbitArgPeriapsis:
      return DisplayData(bankDisplayMode, orbitInfoMsg.argPeriapsis);

    case BankDisplayMode::OrbitTrueAnomaly:
      return DisplayData(bankDisplayMode, orbitInfoMsg.trueAnomaly);

    case BankDisplayMode::OrbitMeanAnomaly:
      return DisplayData(bankDisplayMode, orbitInfoMsg.meanAnomaly);

    case BankDisplayMode::OrbitPeriod:
      return DisplayData(bankDisplayMode, orbitInfoMsg.period);


    case BankDisplayMode::VesselPointingHeading:
      return DisplayData(bankDisplayMode, vesselPointingMsg.heading);

    case BankDisplayMode::VesselPointingPitch:
      return DisplayData(bankDisplayMode, vesselPointingMsg.pitch);

    case BankDisplayMode::VesselPointingRoll:
      return DisplayData(bankDisplayMode, vesselPointingMsg.roll);

    case BankDisplayMode::VesselPointingOrbitalVelocityHeading:
      return DisplayData(bankDisplayMode, vesselPointingMsg.orbitalVelocityHeading);

    case BankDisplayMode::VesselPointingOrbitalVelocityPitch:
      return DisplayData(bankDisplayMode, vesselPointingMsg.orbitalVelocityPitch);

    case BankDisplayMode::VesselPointingSurfaceVelocityHeading:
      return DisplayData(bankDisplayMode, vesselPointingMsg.surfaceVelocityHeading);

    case BankDisplayMode::VesselPointingSurfaceVelocityPitch:
      return DisplayData(bankDisplayMode, vesselPointingMsg.surfaceVelocityPitch);

    // | External Environment |

    case BankDisplayMode::TargetDistance:
      return DisplayData(bankDisplayMode, targetMsg.distance);

    case BankDisplayMode::TargetVelocity:
      return DisplayData(bankDisplayMode, targetMsg.velocity);

    case BankDisplayMode::TargetHeading:
      return DisplayData(bankDisplayMode, targetMsg.heading);

    case BankDisplayMode::TargetPitch:
      return DisplayData(bankDisplayMode, targetMsg.pitch);

    case BankDisplayMode::TargetVelocityHeading:
      return DisplayData(bankDisplayMode, targetMsg.velocityHeading);

    case BankDisplayMode::TargetVelocityPitch:
      return DisplayData(bankDisplayMode, targetMsg.velocityPitch);


    case BankDisplayMode::AtmosphereAirDensity:
      return DisplayData(bankDisplayMode, atmoConditionsMsg.airDensity);

    case BankDisplayMode::AtmosphereTemperature:
      return DisplayData(bankDisplayMode, atmoConditionsMsg.temperature);

    case BankDisplayMode::AtmospherePressure:
      return DisplayData(bankDisplayMode, atmoConditionsMsg.pressure);


    case BankDisplayMode::IntersectsDistanceAtIntersect1:
      return DisplayData(bankDisplayMode, intersectsMsg.distanceAtIntersect1);

    case BankDisplayMode::IntersectsVelocityAtIntersect1:
      return DisplayData(bankDisplayMode, intersectsMsg.velocityAtIntersect1);

    case BankDisplayMode::IntersectsTimeToIntersect1:
      return DisplayData(bankDisplayMode, intersectsMsg.timeToIntersect1);

    case BankDisplayMode::IntersectsDistanceAtIntersect2:
      return DisplayData(bankDisplayMode, intersectsMsg.distanceAtIntersect2);

    case BankDisplayMode::IntersectsVelocityAtIntersect2:
      return DisplayData(bankDisplayMode, intersectsMsg.velocityAtIntersect2);

    case BankDisplayMode::IntersectsIimeToIntersect2:
      return DisplayData(bankDisplayMode, intersectsMsg.timeToIntersect2);
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
          oled12864.gfx.setTextSize(SSD1306::Size::Lg);
          oled12864.gfx.getTextBounds(bankLabel.label, 0, 0, &x1, &y1, &w, &h);

          // Label frame
          uint16_t labelHeight = SSD1306::Size::Lg + h + SSD1306::Size::Lg;
          oled12864.gfx.drawRoundRect(1, 1, oled12864.gfx.width() - 2, labelHeight - 2, SSD1306::Size::Lg + SSD1306::Size::Lg, WHITE);

          // Scale and Units
          oled12864.gfx.setTextSize(SSD1306::Size::Lg);
          oled12864.gfx.setCursor(SSD1306::Size::Lg, SSD1306::Size::Lg);
          oled12864.gfx.print(displayData.scale);
          oled12864.gfx.print(displayData.units);

          // Info
          if (bankIndex == 0)
          {
            oled12864.gfx.setTextSize(SSD1306::Size::Sm);
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
          }

          // Label
          oled12864.gfx.setTextSize(SSD1306::Size::Lg);
          oled12864.gfx.setCursor(oled12864.gfx.width() - w - SSD1306::Size::Lg, SSD1306::Size::Lg);
          oled12864.gfx.printf(bankLabel.label);

          oled12864.gfx.setCursor(SSD1306::Size::Lg, labelHeight + SSD1306::Size::Lg);
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
