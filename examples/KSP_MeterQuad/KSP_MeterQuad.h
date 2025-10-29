// (c) 2025 gogodyne
#ifndef KSP_MeterQuad_h
#define KSP_MeterQuad_h

#include <KerbalSimpit.h>
KerbalSimpit mySimpit(Serial);  // Declare a KerbalSimpit object that will communicate using the "Serial" device.
#include <KontrolRack.h>
#include <KontrolRack_KR_LED24OLED12864.h>
#include "KontrolRack_ESPWiFi.h"

using namespace KontrolRack;

#define BANKSCENE_COUNT (4 + 1)

////////////////////////////////////////////////////////////////////////////////
class KSP_MeterQuad : public KR::Meter24Quad
{
public:
  using Parent = KR::Meter24Quad;

  ESPWiFi net;

  // Bank
  // Modes that a Bank can have
  enum class BankMode : uint8_t
  {
    OFF,
    LF,// liquid fuel
    LF_STAGE,
    OX,// oxidizer
    OX_STAGE,
    SF,// solid fuel
    SF_STAGE,
    XE,// xenon
    XE_STAGE,
    MP,// monopropellant
    EV,// EVA MP
    AI,// intake air
    HF,// hydrogen
    HF_STAGE,
    UF,// uranium
    EL,// electric
    OR,// ore
    AB,// ablator
    AB_STAGE,
    // TR,// TACLS resource
    // TW,// TACLS waste
    // C1,// custom 1
    // C2,// custom 2
    SIZE
  };
  // Bank modes for display
  const char* bankModeNames[(int)BankMode::SIZE] =
  {
    "(off)",
    "Liq Fuel",
    "Liq Fuel (stage)",
    "Oxidizer",
    "Oxidizer (stage)",
    "Solid Fuel",
    "Solid Fuel (stage)",
    "Xenon",
    "Xenon (stage)",
    "Monopropellant",
    "EVA Monopropellant",
    "Intake Air",
    "Hydrogen",
    "Hydrogen (stage)",
    "Uranium",
    "Electric",
    "Ore",
    "Ablator",
    "Ablator (stage)",
    // {"Food", "", "Water", "Oxygen"},
    // {"Waste", "", "Liquid", "CO2"},
    // {"Res1", "", "Res2", "Res3", "Res4"},
    // {"Res5", "", "Res6", "Res7", "Res8"},
  };
  struct BankLabel
  {
    const char* label;
    const char* indicator;
  };
  // Labels per mode
  const BankLabel bankLabels[(int)BankMode::SIZE] =
  {
    {"--", ""},
    {"Lf", ""},
    {"Lf", "STG"},
    {"Ox", ""},
    {"Ox", "STG"},
    {"Sf", ""},
    {"Sf", "STG"},
    {"Xe", ""},
    {"Xe", "STG"},
    {"Mp", ""},
    {"Ev", "EVA"},
    {"Ai", ""},
    {"Hf", ""},
    {"Hf", "STG"},
    {"Uf", ""},
    {"El", ""},
    {"Or", ""},
    {"Ab", ""},
    {"Ab", "STG"},
    // {"Food", "", "Water", "Oxygen"},
    // {"Waste", "", "Liquid", "CO2"},
    // {"Res1", "", "Res2", "Res3", "Res4"},
    // {"Res5", "", "Res6", "Res7", "Res8"},
  };
  // A preset group of Bank modes; one mode per Bank
  struct BankScene
  {
    BankMode mode[bankCount];
  };
  BankScene bankScenes[BANKSCENE_COUNT] =
  {
    {{BankMode::OFF, BankMode::OFF, BankMode::OFF, BankMode::EV}},// EVA[0]
    {{BankMode::LF, BankMode::OX, BankMode::SF, BankMode::MP}},
    {{BankMode::LF_STAGE,BankMode::OX_STAGE, BankMode::SF_STAGE, BankMode::MP}},
    {{BankMode::LF, BankMode::OX, BankMode::XE, BankMode::MP}},
    {{BankMode::LF_STAGE, BankMode::OX_STAGE, BankMode::XE_STAGE, BankMode::MP}},
  };
  uint8_t bankSceneIndex = 1;// Skip EVA[0]

  // Data for display
  struct DisplayData
  {
    BankMode bankMode = BankMode::OFF;
    float total = 0;
    float available = 0;
    float level = 0;// [0-1]
    uint8_t meterValue = 0;// number of LEDs
    uint8_t percent = 0;// [0-100]

    DisplayData()
    : bankMode(BankMode::OFF)
    {}
    DisplayData(BankMode bankMode)
    : bankMode(bankMode)
    {}
    DisplayData(BankMode bankMode, float total, float available)
    : bankMode(bankMode)
    , total(total)
    , available(available)
    {
      if (total)
      {
        level = available / total;
        percent = level * 100;
        meterValue = level * ((float)LED24::getSize() - 1);
      }
    }
    DisplayData(BankMode bankMode, const resourceMessage& message)
    : DisplayData(bankMode, message.total, message.available)
    {}
  };

  // KSP Messages
  void (*mySimpitHandler)(byte messageType, byte msg[], byte msgSize) = nullptr;
  // | Propulsion Resources |
  resourceMessage liquidFuelMsg;
  resourceMessage liquidFuelStageMsg;
  resourceMessage oxidizerMsg;
  resourceMessage oxidizerStageMsg;
  resourceMessage solidFuelMsg;
  resourceMessage solidFuelStageMsg;
  resourceMessage xenonGasMsg;
  resourceMessage xenonGasStageMsg;
  resourceMessage monopropellantMsg;
  resourceMessage evaMonopropellantMsg;
  // | Resources in KSP2 |
  resourceMessage intakeAirMsg;
  resourceMessage hydrogenMsg;
  resourceMessage hydrogenStageMsg;
  resourceMessage uraniumMsg;
  // | Vessel Resources |
  resourceMessage electricMsg;
  resourceMessage oreMsg;
  resourceMessage ablatorMsg;
  resourceMessage ablatorStageMsg;
  // TACLSRessourceMessage taclsResourceMsg;
  // TACLSWasteMessage taclsWasteMsg;
  // CustomResourceMessage customResource1Msg;
  // CustomResourceMessage customResource2Msg;

  // For tracking EVA
  flightStatusMessage flightStatusMsg;

  // Connection
  int connectionState = 0;
  unsigned long pingMs = 0;

  KSP_MeterQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin(void (*messageHandler)(byte messageType, byte msg[], byte msgSize))
  {
    mySimpitHandler = messageHandler;

    for (int i = 0; i < getBankSize(); ++i)
    {
      // Start the devices.
      led24Devices[i].begin(12);
      oled12864Devices[i].begin(12);
    }

    Parent::begin(12, false, SWITCH_ADDRESS_METER, OLED12864_ADDRESS, LED24::Info(LED24_ADDRESS, &wire, LED24_Brightness), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));
    net.begin();
  }

  virtual void loop() override
  {
    Parent::loop();
    net.loop();

    if (encBtn.didPress())
    {
      ESP.restart();
    }
    if (encBtn.didIncrease())
    {
      bankSceneIndex = min(bankSceneIndex + 1, BANKSCENE_COUNT - 1);
    }
    if (encBtn.didDecrease())
    {
      bankSceneIndex = max(bankSceneIndex - 1, 1);// Skip EVA[0]
    }

    tryConnect();
    mySimpit.update();
  }

  //------------------------------------------------------------------------------
  // Connection

  virtual int tryConnect()
  {
    unsigned long ms = millis();

    // Ping
    if (ms >= pingMs)
    {
      pingMs = ms + 3000;
      char buf[48] = {0};
      snprintf(buf, 48, "'%s' PING\n", net.hostName);
      mySimpit.send(ECHO_REQ_MESSAGE, buf, strlen(buf) + 1);
    }

    // Connect
    static unsigned long tryConnectMs = 0;
    if ((connectionState == 0) && (ms >= tryConnectMs))
    {
      tryConnectMs = ms + 3000;
      connectionState = mySimpit.init();
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

    // | Propulsion Resources |
    mySimpit.registerChannel(LF_MESSAGE);
    mySimpit.registerChannel(LF_STAGE_MESSAGE);
    mySimpit.registerChannel(OX_MESSAGE);
    mySimpit.registerChannel(OX_STAGE_MESSAGE);
    mySimpit.registerChannel(SF_MESSAGE);
    mySimpit.registerChannel(SF_STAGE_MESSAGE);
    mySimpit.registerChannel(XENON_GAS_MESSAGE);
    mySimpit.registerChannel(XENON_GAS_STAGE_MESSAGE);
    mySimpit.registerChannel(MONO_MESSAGE);
    mySimpit.registerChannel(EVA_MESSAGE);
    // | KSP2 only Resources |
    mySimpit.registerChannel(INTAKE_AIR_MESSAGE);
    mySimpit.registerChannel(HYDROGEN_MESSAGE);
    mySimpit.registerChannel(HYDROGEN_STAGE_MESSAGE);
    mySimpit.registerChannel(URANIUM_MESSAGE);
    // | Vessel Resources |
    mySimpit.registerChannel(ELECTRIC_MESSAGE);
    mySimpit.registerChannel(ORE_MESSAGE);
    mySimpit.registerChannel(AB_MESSAGE);
    mySimpit.registerChannel(AB_STAGE_MESSAGE);
    // mySimpit.registerChannel(TACLS_RESOURCE_MESSAGE);
    // mySimpit.registerChannel(TACLS_WASTE_MESSAGE);
    // mySimpit.registerChannel(CUSTOM_RESOURCE_1_MESSAGE);
    // mySimpit.registerChannel(CUSTOM_RESOURCE_2_MESSAGE);

    // To track EVA
    mySimpit.registerChannel(FLIGHT_STATUS_MESSAGE);
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

    case FLIGHT_STATUS_MESSAGE:
      {
        if (msgSize == sizeof(flightStatusMessage))
        {
          flightStatusMsg = parseMessage<flightStatusMessage>(msg);
        }
      }
      break;

    // | Propulsion Resources |

    case LF_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          liquidFuelMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case LF_STAGE_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          liquidFuelStageMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case OX_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          oxidizerMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case OX_STAGE_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          oxidizerStageMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case SF_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          solidFuelMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case SF_STAGE_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          solidFuelStageMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case XENON_GAS_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          xenonGasMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case XENON_GAS_STAGE_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          xenonGasStageMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case MONO_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          monopropellantMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case EVA_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          evaMonopropellantMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    // | KSP2 only Resources |

    case INTAKE_AIR_MESSAGE:
      { //Only works on KSP2
        if (msgSize == sizeof(resourceMessage))
        {
          intakeAirMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case HYDROGEN_MESSAGE:
      { //Only works on KSP2
        if (msgSize == sizeof(resourceMessage))
        {
          hydrogenMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case HYDROGEN_STAGE_MESSAGE:
      { //Only works on KSP2
        if (msgSize == sizeof(resourceMessage))
        {
          hydrogenStageMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case URANIUM_MESSAGE:
      { //Only works on KSP2
        if (msgSize == sizeof(resourceMessage))
        {
          uraniumMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    // | Vessel Resources |

    case ELECTRIC_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          electricMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case ORE_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          oreMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case AB_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          ablatorMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    case AB_STAGE_MESSAGE:
      { // (Note: For this to work on KSP1 this needs the mod ARP to be installed)
        if (msgSize == sizeof(resourceMessage))
        {
          ablatorStageMsg = parseMessage<resourceMessage>(msg);
        }
      }
      break;

    // case TACLS_RESOURCE_MESSAGE:
    //   { // (Note: Only works on KSP1. This needs the mod ARP and TAC Life Support to be installed)
    //     if (msgSize == sizeof(TACLSRessourceMessage))
    //     {
    //       TACLSRessourceMessage taclsResourceMsg = parseMessage<TACLSRessourceMessage>(msg);
    //     }
    //   }
    //   break;

    // case TACLS_WASTE_MESSAGE:
    //   { // (Note: Only works on KSP1. This needs the mod ARP and TAC Life Support to be installed)
    //     if (msgSize == sizeof(TACLSWasteMessage))
    //     {
    //       TACLSWasteMessage taclsWasteMsg = parseMessage<TACLSWasteMessage>(msg);
    //     }
    //   } break;

    // case CUSTOM_RESOURCE_1_MESSAGE:
    //   { // (Note: Only works on KSP1. This needs the mod ARP and Community Resource Pack to be installed)
    //     if (msgSize == sizeof(CustomResourceMessage))
    //     {
    //       CustomResourceMessage customResource1Msg = parseMessage<CustomResourceMessage>(msg);
    //     }
    //   }
    //   break;

    // case CUSTOM_RESOURCE_2_MESSAGE:
    //   { // (Note: Only works on KSP1. This needs the mod ARP and Community Resource Pack to be installed)
    //     if (msgSize == sizeof(CustomResourceMessage))
    //     {
    //       CustomResourceMessage customResource2Msg = parseMessage<CustomResourceMessage>(msg);
    //     }
    //   }
    //   break;
    }
  }

  virtual DisplayData makeDisplayData(uint8_t bankIndex)
  {
    BankMode bankMode = flightStatusMsg.isInEVA() ? BankMode::EV : bankScenes[bankSceneIndex].mode[bankIndex];

    switch (bankMode)
    {
    default:
    case BankMode::OFF:
      return DisplayData(bankMode);

    // | Propulsion Resources |

    case BankMode::LF:
      return DisplayData(bankMode, liquidFuelMsg);

    case BankMode::LF_STAGE:
      return DisplayData(bankMode, liquidFuelStageMsg);

    case BankMode::OX:
      return DisplayData(bankMode, oxidizerMsg);

    case BankMode::OX_STAGE:
      return DisplayData(bankMode, oxidizerStageMsg);

    case BankMode::SF:
      return DisplayData(bankMode, solidFuelMsg);

    case BankMode::SF_STAGE:
      return DisplayData(bankMode, solidFuelStageMsg);

    case BankMode::XE:
      return DisplayData(bankMode, xenonGasMsg);

    case BankMode::XE_STAGE:
      return DisplayData(bankMode, xenonGasStageMsg);

    case BankMode::MP:
      return DisplayData(bankMode, monopropellantMsg);

    case BankMode::EV:
      return DisplayData(bankMode, evaMonopropellantMsg);

    // | KSP2 only Resources |

    case BankMode::AI:
      return DisplayData(bankMode, intakeAirMsg);

    case BankMode::HF:
      return DisplayData(bankMode, hydrogenMsg);

    case BankMode::HF_STAGE:
      return DisplayData(bankMode, hydrogenStageMsg);

    case BankMode::UF:
      return DisplayData(bankMode, uraniumMsg);

    // | Vessel Resources |

    case BankMode::EL:
      return DisplayData(bankMode, electricMsg);

    case BankMode::OR:
      return DisplayData(bankMode, oreMsg);

    case BankMode::AB:
      return DisplayData(bankMode, ablatorMsg);

    case BankMode::AB_STAGE:
      return DisplayData(bankMode, ablatorStageMsg);

    // case BankMode::T:
    // case BankMode::TW:
    // case BankMode::C1:
    // case BankMode::C2:
    }
  }

  //------------------------------------------------------------------------------
  // Drawing

  virtual bool isDrawBankTick(uint8_t bankIndex) override
  {
    return oled12864Devices[bankIndex].timing.isTick || led24Devices[bankIndex].timing.isTick;
  }

  virtual void drawBank(uint8_t bankIndex, bool isDirty) override
  {
    Parent::drawBank(bankIndex, isDirty);

    const int LABELSIZE = 4;
    const int LABELSIZE_SM = 2;
    DisplayData displayData = makeDisplayData(bankIndex);
    BankLabel bankLabel = bankLabels[(int)displayData.bankMode];

    // OLED
    if (isDirty || oled12864Devices[bankIndex].timing.isTick)
    {
      oled12864.clear();
      drawBankInverted(bankIndex, oled12864Devices[bankIndex].timing.isHz(1.f/30.f));
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

          // Label
          oled12864.gfx.setTextSize(LABELSIZE);
          oled12864.gfx.setCursor((oled12864.gfx.width() - w) / 2, LABELSIZE);
          oled12864.gfx.print(bankLabel.label);

          // Indicators
          oled12864.gfx.setTextSize(LABELSIZE_SM);
          oled12864.gfx.setCursor(LABELSIZE, LABELSIZE);
          {
            // Bank Scene index
            oled12864.gfx.printf("%d", bankSceneIndex);

            // Connection status
            if (connectionState < 1)
            {
              oled12864.gfx.print((connectionState < 0) ? "*" : timing.isHz(2) ? "/" : "\\");
            }
            oled12864.gfx.println();
            oled12864.gfx.setCursor(oled12864.gfx.getCursorX() + LABELSIZE, oled12864.gfx.getCursorY());

            // Indicator/Stage
            if (bankLabel.indicator && strlen(bankLabel.indicator))
            {
              oled12864.gfx.print((timing.isHz(1) || timing.isHz(.5)) ? bankLabel.indicator : "---");
            }
          }

          // Available/Total
          if (displayData.total)
          {
            char text[32];
            text[0] = 0;
            oled12864.gfx.setTextSize(LABELSIZE_SM);
            oled12864.gfx.setCursor(LABELSIZE, labelHeight + LABELSIZE);
            if (timing.isHz(1.f/3.f))
            {
              oled12864.gfx.print("+");
              // Available
              sprintf(text, "%.1f", displayData.available);
            }
            else
            {
              oled12864.gfx.print("/");
              // Total
              sprintf(text, "%.1f", displayData.total);
            }
            oled12864.gfx.setTextSize(LABELSIZE_SM);
            oled12864.gfx.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
            oled12864.gfx.setCursor(oled12864.gfx.width() - w - LABELSIZE, labelHeight + LABELSIZE);
            oled12864.gfx.print(text);
          }
        }
      }
      oled12864.render();
    }

    // LED24
    if (isDirty || led24Devices[bankIndex].timing.isTick)
    {
      led24.clear();
      drawLedHighlight(bankIndex);
      {
        if (displayData.total)
        {
          for (uint8_t led = 0; led < LED24::getSize(); ++led)
          {
            if (led > displayData.meterValue + 1)
              led24.setBar(led, LED_RED);
            if (led == displayData.meterValue + 1)
              led24.setBar(led, LED_OFF);
            if (led == displayData.meterValue)
              led24.setBar(led, LED_YELLOW);
            if (led == displayData.meterValue - 1)
              led24.setBar(led, LED_OFF);
            if (led < displayData.meterValue - 1)
              led24.setBar(led, LED_GREEN);
          }
        }
        else
        {
          led24.setBar(0, LED_YELLOW);
        }
      }
      led24.render();
    }
  }
};

#endif// KSP_MeterQuad_h
