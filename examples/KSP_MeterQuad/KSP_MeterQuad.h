// (c) 2025 gogodyne
#ifndef KSP_MeterQuad_h
#define KSP_MeterQuad_h

#include <Preferences.h>
#include <KerbalSimpit.h>
KerbalSimpit mySimpit(Serial);  // Declare a KerbalSimpit object that will communicate using the "Serial" device.
#include <KontrolRack.h>
#include <KontrolRack_KR_LED24OLED12864.h>
#include "KontrolRack_ESPWiFi.h"

using namespace KontrolRack;

#define BANKSCENE_COUNT (4 + 1)

////////////////////////////////////////////////////////////////////////////////
#define BANKSCENE_INDEX_DEFAULT 1// skip EVA[0]
#define BANKSCENE_INDEX_KEY "bankSceneIndex"
#define BANKSCENE_ROW_KEYFORMAT "row%d"
#define s_KSP_MeterQuad "KSP_MeterQuad"// max 15 characters
#define PREFS_Namespace s_KSP_MeterQuad
class KSP_MeterQuad : public KR::Meter24Quad
{
public:
  using Parent = KR::Meter24Quad;

  ESPWiFi net;

  // Bank
  // Modes that a Bank can have
  enum class BankDisplayMode : uint8_t
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
    IA,// intake air
    HF,// hydrogen
    HF_STAGE,
    UF,// uranium
    EL,// electric
    OR,// ore
    AB,// ablator
    AB_STAGE,
    TR_FOOD,// TACLS resource
    TR_WATER,// TACLS resource
    TR_AIR,// TACLS resource
    TW_WASTESOLID,// TACLS waste
    TW_WASTELIQUID,// TACLS waste
    TW_WASTEGAS,// TACLS waste
    C1,// custom resource
    C2,// custom resource
    C3,// custom resource
    C4,// custom resource
    C5,// custom resource
    C6,// custom resource
    C7,// custom resource
    C8,// custom resource
    SIZE
  };
  struct BankLabel
  {
    const char* name;
    const char* label;
    const char* indicator;
  };
  // Labels per mode
  const BankLabel bankLabels[(int)BankDisplayMode::SIZE] =
  {
    {"(off)",               "--",     ""},
    {"Liquid Fuel",         "Lf",     ""},
    {"Liquid Fuel (stage)", "Lf",     "STG"},
    {"Oxidizer",            "Ox",     ""},
    {"Oxidizer (stage)",    "Ox",     "STG"},
    {"Solid Fuel",          "Sf",     ""},
    {"Solid Fuel (stage)",  "Sf",     "STG"},
    {"Xenon",               "Xe",     ""},
    {"Xenon (stage)",       "Xe",     "STG"},
    {"Monopropellant",      "Mp",     ""},
    {"EVA Monopropellant",  "Ev",     "EVA"},
    {"Intake Air",          "Ia",     ""},
    {"Hydrogen",            "Hf",     ""},
    {"Hydrogen (stage)",    "Hf",     "STG"},
    {"Uranium",             "Uf",     ""},
    {"Electric",            "El",     ""},
    {"Ore",                 "Or",     ""},
    {"Ablator",             "Ab",     ""},
    {"Ablator (stage)",     "Ab",     "STG"},
    {"Food",                "Food",   ""},
    {"Water",               "H2O",    ""},
    {"Air",                 "Air",    ""},
    {"Solid Waste",         "wSol",   ""},
    {"Liquid Waste",        "wLiq",   ""},
    {"Gas Waste",           "wGas",   ""},
    {"Resource 1",          "R1",     ""},
    {"Resource 2",          "R2",     ""},
    {"Resource 3",          "R3",     ""},
    {"Resource 4",          "R4",     ""},
    {"Resource 5",          "R5",     ""},
    {"Resource 6",          "R6",     ""},
    {"Resource 7",          "R7",     ""},
    {"Resource 8",          "R8",     ""},
  };
  // A preset group of Bank modes; one mode per Bank
  struct BankScene
  {
    BankDisplayMode modes[bankCount];
  };
  BankScene bankScenes[BANKSCENE_COUNT] =
  {
    {{BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::EV}},// EVA[0]
    {{BankDisplayMode::LF, BankDisplayMode::OX, BankDisplayMode::SF, BankDisplayMode::MP}},
    {{BankDisplayMode::LF_STAGE,BankDisplayMode::OX_STAGE, BankDisplayMode::SF_STAGE, BankDisplayMode::MP}},
    {{BankDisplayMode::LF, BankDisplayMode::OX, BankDisplayMode::XE, BankDisplayMode::MP}},
    {{BankDisplayMode::LF_STAGE, BankDisplayMode::OX_STAGE, BankDisplayMode::XE_STAGE, BankDisplayMode::MP}},
  };
  uint8_t bankSceneIndex = BANKSCENE_INDEX_DEFAULT;// Skip EVA[0]

  // Data for display
  struct DisplayData
  {
    BankDisplayMode bankDisplayMode = BankDisplayMode::OFF;
    float total = 0;
    float available = 0;
    float level = 0;// [0-1]
    uint8_t meterValue = 0;// number of LEDs
    uint8_t percent = 0;// [0-100]

    DisplayData()
    : bankDisplayMode(BankDisplayMode::OFF)
    {}
    DisplayData(BankDisplayMode bankDisplayMode)
    : bankDisplayMode(bankDisplayMode)
    {}
    DisplayData(BankDisplayMode bankDisplayMode, float total, float available)
    : bankDisplayMode(bankDisplayMode)
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
    DisplayData(BankDisplayMode bankDisplayMode, const resourceMessage& message)
    : DisplayData(bankDisplayMode, message.total, message.available)
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
  TACLSResourceMessage taclsResourceMsg;
  TACLSWasteMessage taclsWasteMsg;
  CustomResourceMessage customResource1Msg;
  CustomResourceMessage customResource2Msg;

  // For tracking EVA
  flightStatusMessage flightStatusMsg;

  // Connection
  int connectionState = 0;
  unsigned long heartbeatNextMs = 0;

  Preferences preferences;

  KSP_MeterQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin(void (*messageHandler)(byte messageType, byte msg[], byte msgSize))
  {
    prefsBegin();

    mySimpitHandler = messageHandler;

    for (int i = 0; i < getBankCount(); ++i)
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
      bankSceneIndex = max(bankSceneIndex - 1, 1);// Skip EVA[0]
    }
  }

  virtual void prefsBegin()
  {
    preferences.begin(PREFS_Namespace);
    prefs(true);
  }

  virtual void prefsStore()
  {
    prefs(false);
  }

  virtual void prefs(bool load)
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
    mySimpit.registerChannel(TACLS_RESOURCE_MESSAGE);
    mySimpit.registerChannel(TACLS_WASTE_MESSAGE);
    mySimpit.registerChannel(CUSTOM_RESOURCE_1_MESSAGE);
    mySimpit.registerChannel(CUSTOM_RESOURCE_2_MESSAGE);

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

    case TACLS_RESOURCE_MESSAGE:
      { // (Note: Only works on KSP1. This needs the mod ARP and TAC Life Support to be installed)
        if (msgSize == sizeof(TACLSResourceMessage))
        {
          taclsResourceMsg = parseMessage<TACLSResourceMessage>(msg);
        }
      }
      break;

    case TACLS_WASTE_MESSAGE:
      { // (Note: Only works on KSP1. This needs the mod ARP and TAC Life Support to be installed)
        if (msgSize == sizeof(TACLSWasteMessage))
        {
          taclsWasteMsg = parseMessage<TACLSWasteMessage>(msg);
        }
      } break;

    case CUSTOM_RESOURCE_1_MESSAGE:
      { // (Note: Only works on KSP1. This needs the mod ARP and Community Resource Pack to be installed)
        if (msgSize == sizeof(CustomResourceMessage))
        {
          customResource1Msg = parseMessage<CustomResourceMessage>(msg);
        }
      }
      break;

    case CUSTOM_RESOURCE_2_MESSAGE:
      { // (Note: Only works on KSP1. This needs the mod ARP and Community Resource Pack to be installed)
        if (msgSize == sizeof(CustomResourceMessage))
        {
          customResource2Msg = parseMessage<CustomResourceMessage>(msg);
        }
      }
      break;
    }
  }

  //------------------------------------------------------------------------------
  // Drawing

  virtual DisplayData makeDisplayData(uint8_t bankIndex)
  {
    BankDisplayMode bankDisplayMode = flightStatusMsg.isInEVA() ? BankDisplayMode::EV : bankScenes[bankSceneIndex].modes[bankIndex];

    switch (bankDisplayMode)
    {
    default:
    case BankDisplayMode::OFF:
      return DisplayData(bankDisplayMode);

    // | Propulsion Resources |

    case BankDisplayMode::LF:
      return DisplayData(bankDisplayMode, liquidFuelMsg);

    case BankDisplayMode::LF_STAGE:
      return DisplayData(bankDisplayMode, liquidFuelStageMsg);

    case BankDisplayMode::OX:
      return DisplayData(bankDisplayMode, oxidizerMsg);

    case BankDisplayMode::OX_STAGE:
      return DisplayData(bankDisplayMode, oxidizerStageMsg);

    case BankDisplayMode::SF:
      return DisplayData(bankDisplayMode, solidFuelMsg);

    case BankDisplayMode::SF_STAGE:
      return DisplayData(bankDisplayMode, solidFuelStageMsg);

    case BankDisplayMode::XE:
      return DisplayData(bankDisplayMode, xenonGasMsg);

    case BankDisplayMode::XE_STAGE:
      return DisplayData(bankDisplayMode, xenonGasStageMsg);

    case BankDisplayMode::MP:
      return DisplayData(bankDisplayMode, monopropellantMsg);

    case BankDisplayMode::EV:
      return DisplayData(bankDisplayMode, evaMonopropellantMsg);

    // | KSP2 only Resources |

    case BankDisplayMode::IA:
      return DisplayData(bankDisplayMode, intakeAirMsg);

    case BankDisplayMode::HF:
      return DisplayData(bankDisplayMode, hydrogenMsg);

    case BankDisplayMode::HF_STAGE:
      return DisplayData(bankDisplayMode, hydrogenStageMsg);

    case BankDisplayMode::UF:
      return DisplayData(bankDisplayMode, uraniumMsg);

    // | Vessel Resources |

    case BankDisplayMode::EL:
      return DisplayData(bankDisplayMode, electricMsg);

    case BankDisplayMode::OR:
      return DisplayData(bankDisplayMode, oreMsg);

    case BankDisplayMode::AB:
      return DisplayData(bankDisplayMode, ablatorMsg);

    case BankDisplayMode::AB_STAGE:
      return DisplayData(bankDisplayMode, ablatorStageMsg);

    case BankDisplayMode::TR_FOOD:
      return DisplayData(bankDisplayMode, taclsResourceMsg.maxFood, taclsResourceMsg.currentFood);

    case BankDisplayMode::TR_WATER:
      return DisplayData(bankDisplayMode, taclsResourceMsg.maxWater, taclsResourceMsg.currentWater);

    case BankDisplayMode::TR_AIR:
      return DisplayData(bankDisplayMode, taclsResourceMsg.maxOxygen, taclsResourceMsg.currentOxygen);

    case BankDisplayMode::TW_WASTESOLID:
      return DisplayData(bankDisplayMode, taclsWasteMsg.maxWaste, taclsWasteMsg.currentWaste);

    case BankDisplayMode::TW_WASTELIQUID:
      return DisplayData(bankDisplayMode, taclsWasteMsg.maxLiquidWaste, taclsWasteMsg.currentLiquidWaste);

    case BankDisplayMode::TW_WASTEGAS:
      return DisplayData(bankDisplayMode, taclsWasteMsg.maxCO2, taclsWasteMsg.currentCO2);

    case BankDisplayMode::C1:
      return DisplayData(bankDisplayMode, customResource1Msg.maxResource1, customResource1Msg.currentResource1);

    case BankDisplayMode::C2:
      return DisplayData(bankDisplayMode, customResource1Msg.maxResource2, customResource1Msg.currentResource2);

    case BankDisplayMode::C3:
      return DisplayData(bankDisplayMode, customResource1Msg.maxResource3, customResource1Msg.currentResource3);

    case BankDisplayMode::C4:
      return DisplayData(bankDisplayMode, customResource1Msg.maxResource4, customResource1Msg.currentResource4);

    case BankDisplayMode::C5:
      return DisplayData(bankDisplayMode, customResource2Msg.maxResource1, customResource2Msg.currentResource1);

    case BankDisplayMode::C6:
      return DisplayData(bankDisplayMode, customResource2Msg.maxResource2, customResource2Msg.currentResource2);

    case BankDisplayMode::C7:
      return DisplayData(bankDisplayMode, customResource2Msg.maxResource3, customResource2Msg.currentResource3);

    case BankDisplayMode::C8:
      return DisplayData(bankDisplayMode, customResource2Msg.maxResource4, customResource2Msg.currentResource4);
    }
  }

  virtual bool isDrawBankTick(uint8_t bankIndex) override
  {
    return oled12864Devices[bankIndex].timing.isTick || led24Devices[bankIndex].timing.isTick;
  }

  virtual void drawBank(uint8_t bankIndex, bool isDirty) override
  {
    Parent::drawBank(bankIndex, isDirty);

    const int LABELSIZE = 4;
    const int LABELSIZE_M = 2;
    const int LABELSIZE_SM = 1;

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

          // Label
          oled12864.gfx.setTextSize(LABELSIZE);
          oled12864.gfx.setCursor((oled12864.gfx.width() - w) / 2, LABELSIZE);
          oled12864.gfx.print(bankLabel.label);

          // Indicators
          oled12864.gfx.setTextSize(LABELSIZE_M);
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

          oled12864.gfx.setCursor(LABELSIZE, labelHeight + LABELSIZE);
          if (bankSelectMode == KR::BankSelectMode::None)
          {
            // Available/Total
            if (displayData.total)
            {
              char text[32];
              text[0] = 0;
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
              oled12864.gfx.setTextSize(LABELSIZE_M);
              oled12864.gfx.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
              oled12864.gfx.setCursor(oled12864.gfx.width() - w - LABELSIZE, labelHeight + LABELSIZE);
              oled12864.gfx.print(text);
            }
          }
          else
          // Editing
          {
            oled12864.gfx.setTextSize(LABELSIZE_SM);
            // Bank Mode name
            oled12864.gfx.print(bankLabel.name);
          }
        }
      }
      drawOledEffects(bankIndex);
      oled12864.render();
    }

    // LED24
    if (isDirty || led24Devices[bankIndex].timing.isTick)
    {
      led24.clear();
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
      drawLed24Effects(bankIndex);
      led24.render();
    }
  }
};

#endif// KSP_MeterQuad_h
