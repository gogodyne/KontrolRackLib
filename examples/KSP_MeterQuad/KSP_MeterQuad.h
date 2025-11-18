// (c) 2025 gogodyne
#ifndef KSP_MeterQuad_h
#define KSP_MeterQuad_h

#include <Preferences.h>
#include <nvs_flash.h>
#include <KerbalSimpit.h>
KerbalSimpit mySimpit(Serial);  // Declare a KerbalSimpit object that will communicate using the "Serial" device.
#include <KontrolRack.h>
#include <KontrolRack_KR_LED24OLED12864.h>
#include "KSP_MeterQuadWeb.h"

// Use this to CLEAR *ALL* NVS and freeze
// #define PREFS_CLEAR

using namespace KontrolRack;

#define SCENE_COUNT (16)
#define SCENE_INDEX_KEY "bankSceneIndex"
#define SCENE_ROW_KEYFORMAT "row%d"

////////////////////////////////////////////////////////////////////////////////
#define s_KSP_MeterQuad "KSP_MeterQuad"// max 15 characters
#define PREFS_Namespace s_KSP_MeterQuad
class KSP_MeterQuad : public KR::Meter24Quad
{
public:
  using Parent = KR::Meter24Quad;

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
    const char* sublabel;
  };

  // Labels per mode
  const BankLabel bankLabels[(int)BankDisplayMode::SIZE] =
  {
    {"(off)",               "",     ""},

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
    {"Intake Air",          "In",     ""},
    {"Hydrogen Fuel",       "Hf",     ""},
    {"Hydrogen Fl (stage)", "Hf",     "STG"},
    {"Uranium",             "Ur",     ""},
    {"Electric",            "El",     ""},
    {"Ore",                 "Ore",    ""},
    {"Ablator",             "Ab",     ""},
    {"Ablator (stage)",     "Ab",     "STG"},
    {"Food",                "Food",   ""},
    {"Water",               "H2O",    ""},
    {"Air",                 "Air",    ""},
    {"Solid Waste",         "wSol",   ""},
    {"Liquid Waste",        "wLiq",   ""},
    {"Gas Waste",           "wGas",   ""},
    {"Resource 1",          "Res1",   ""},
    {"Resource 2",          "Res2",   ""},
    {"Resource 3",          "Res3",   ""},
    {"Resource 4",          "Res4",   ""},
    {"Resource 5",          "Res5",   ""},
    {"Resource 6",          "Res6",   ""},
    {"Resource 7",          "Res7",   ""},
    {"Resource 8",          "Res8",   ""},
  };

  // A preset group of Bank modes; one mode per Bank
  struct BankScene
  {
    BankDisplayMode modes[bankCount];
  };
  BankScene bankScenes[SCENE_COUNT] =
  {
    {{BankDisplayMode::LF_STAGE, BankDisplayMode::OX_STAGE, BankDisplayMode::SF_STAGE, BankDisplayMode::MP}},
    {{BankDisplayMode::LF, BankDisplayMode::OX, BankDisplayMode::SF, BankDisplayMode::MP}},
    {{BankDisplayMode::LF_STAGE, BankDisplayMode::OX_STAGE, BankDisplayMode::EL, BankDisplayMode::MP}},
    {{BankDisplayMode::LF, BankDisplayMode::OX, BankDisplayMode::EL, BankDisplayMode::MP}},

    {{BankDisplayMode::OFF, BankDisplayMode::AB_STAGE, BankDisplayMode::EL, BankDisplayMode::MP}},
    {{BankDisplayMode::OFF, BankDisplayMode::AB, BankDisplayMode::EL, BankDisplayMode::MP}},
    {{BankDisplayMode::XE_STAGE, BankDisplayMode::OR, BankDisplayMode::EL, BankDisplayMode::MP}},
    {{BankDisplayMode::XE, BankDisplayMode::OR, BankDisplayMode::EL, BankDisplayMode::MP}},

    {{BankDisplayMode::LF_STAGE, BankDisplayMode::IA, BankDisplayMode::EL, BankDisplayMode::OFF}},
    {{BankDisplayMode::LF, BankDisplayMode::IA, BankDisplayMode::EL, BankDisplayMode::OFF}},
    {{BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF}},
    {{BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF, BankDisplayMode::OFF}},

    {{BankDisplayMode::TR_FOOD, BankDisplayMode::TR_WATER, BankDisplayMode::TR_AIR, BankDisplayMode::OFF}},
    {{BankDisplayMode::TW_WASTESOLID, BankDisplayMode::TW_WASTELIQUID, BankDisplayMode::TW_WASTEGAS, BankDisplayMode::OFF}},
    {{BankDisplayMode::C1, BankDisplayMode::C2, BankDisplayMode::C3, BankDisplayMode::C4}},
    {{BankDisplayMode::C5, BankDisplayMode::C6, BankDisplayMode::C7, BankDisplayMode::C8}},
  };
  uint8_t bankSceneIndex = 0;

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

  Preferences preferences;

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
  KSP_MeterQuadWeb web;

  KSP_MeterQuad(TwoWire& inWire)
  : Parent(inWire)
  {}

  using Parent::begin;
  virtual void begin(void (*messageHandler)(byte messageType, byte msg[], byte msgSize))
  {
    prefsBegin();
    web.begin(
      [this](const String& var) { return Web_ParsePage(var); },
      [this](AsyncWebServerRequest* request) { return Web_ProcessForm(request); }
      );

    mySimpitHandler = messageHandler;

    for (int i = 0; i < getBankCount(); ++i)
    {
      // Start the devices
      led24Devices[i].begin(LED24_FPS);
      oled12864Devices[i].begin(OLED12864_FPS);
    }

    Parent::begin(MODULE_FPS, false, SWITCH_ADDRESS_METER, OLED12864_ADDRESS, LED24::Info(LED24_ADDRESS, &wire, LED24_Brightness), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));

#if defined(WIFI_AUTOSTART)
    {
      Serial.print(web.net.hostName);
      Serial.println(": Using WiFi auto start.");
      web.net.WiFi_ConnectAP();
    }
#endif// defined(WIFI_AUTOSTART)
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
  // Input

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
  // Edit

  virtual bool checkBankSelectModeTimeout() override
  {
    if (Parent::checkBankSelectModeTimeout())
    {
      prefsStore();

      return true;
    }

    return false;
  }

  virtual bool isValidScene(int iScene)
  {
    return iScene == constrain(iScene, 0, SCENE_COUNT - 1);
  }

  virtual bool isValidBank(int iBank)
  {
    return iBank == constrain(iBank, 0, bankCount - 1);
  }

  virtual bool isValidBankDisplayMode(int iBankMode)
  {
    return iBankMode == constrain(iBankMode, 0, (int)BankDisplayMode::SIZE - 1);
  }

  virtual bool setBankDisplayMode(int iScene, int iBank, int iBankMode)
  {
    if (isValidScene(iScene) && isValidBank(iBank) && isValidBankDisplayMode(iBankMode))
    {
      if (bankScenes[iScene].modes[iBank] != (BankDisplayMode)iBankMode)
      {
        bankScenes[iScene].modes[iBank] = (BankDisplayMode)iBankMode;

        return true;
      }
    }

    return false;
  }

  virtual bool cycleBankDisplayMode(bool next)
  {
    resetBankSelectModeTimeout();

    return setBankDisplayMode(bankSceneIndex, bankSelectedIndex, (int)bankScenes[bankSceneIndex].modes[bankSelectedIndex] + (next ? 1 : -1));
  }

  virtual void cycleBankScene(bool next)
  {
    if (next)
    {
      bankSceneIndex = min(bankSceneIndex + 1, SCENE_COUNT - 1);
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
#if defined(PREFS_CLEAR)
    nvs_flash_erase();
    nvs_flash_init();
    while(true);
#endif// defined(PREFS_CLEAR)

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
      const char* key = SCENE_INDEX_KEY;
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
      char key[16] = { 0 };

      for (int i = 0; i < SCENE_COUNT; ++i)
      {
        sprintf(key, SCENE_ROW_KEYFORMAT, i);
        uint8_t modes[bankCount] = { 0 };
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
      Serial.println();
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
        if (!kspStatus.isConnected())
        {
          onConnect();
        }
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
    BankDisplayMode bankDisplayMode = bankScenes[bankSceneIndex].modes[bankIndex];

    // Show only EVA?
    if (flightStatusMsg.isInEVA())
    {
      bankDisplayMode = (bankIndex == (bankCount - 1)) ? BankDisplayMode::EV : BankDisplayMode::OFF;
    }

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

  virtual void drawMenu(uint8_t bankIndex)
  {
    if (menu.isCfg())
    {
      if (bankIndex == 0)
      {
        oled12864.gfx.setTextSize(SSD1306::Size::Xs, SSD1306::Size::Sm);
        oled12864.gfx.setCursor(0, 0);

        drawMenuItem("Done", menu.isCfg(Menu::Cfg::Done));
        drawMenuItem((kspStatus.isConnecting() ?    "Stop Connectng to KSP" : "Try Connectng to KSP"), menu.isCfg(Menu::Cfg::Connect));
        drawMenuItem((web.net.WiFi_IsConnected() ?  "Stop Web Config" :       "Start Web Config"), menu.isCfg(Menu::Cfg::Web));

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
    return oled12864Devices[bankIndex].timing.isTick || led24Devices[bankIndex].timing.isTick;
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

          // Label
          oled12864.gfx.setTextSize((strlen(bankLabel.label) > 2) ? SSD1306::Size::Md : SSD1306::Size::Lg);
          oled12864.gfx.getTextBounds(bankLabel.label, 0, 0, &x1, &y1, &w, &h);
          oled12864.gfx.setCursor((oled12864.gfx.width() - w) / 2, margin);
          oled12864.gfx.print(bankLabel.label);

          // Indicators
          oled12864.gfx.setTextSize(SSD1306::Size::Sm);
          oled12864.gfx.setCursor(margin / 2, margin / 2);
          {
            // Info
            if (bankIndex == 0)
            {
              oled12864.gfx.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
              {
                // Bank Scene index
                oled12864.gfx.printf("%X", bankSceneIndex);

                // Connection status
                if (!kspStatus.isConnected())
                {
                  oled12864.gfx.print(kspStatus.isConnecting() ? (timing.isHz(2) ? "/" : "\\") : "!");
                }
              }
              oled12864.gfx.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }

            oled12864.gfx.println();

            // Indicator/Stage
            oled12864.gfx.setCursor(oled12864.gfx.getCursorX() + margin, oled12864.gfx.getCursorY() + margin / 2);
            if (bankLabel.sublabel && strlen(bankLabel.sublabel))
            {
              oled12864.gfx.print((timing.isHz(1) || timing.isHz(.5)) ? bankLabel.sublabel : "---");
            }
          }

          oled12864.gfx.setCursor(margin, labelHeight + margin);
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
              oled12864.gfx.setTextSize(SSD1306::Size::Sm);
              oled12864.gfx.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
              oled12864.gfx.setCursor(oled12864.gfx.width() - w - margin, labelHeight + margin);
              oled12864.gfx.print(text);
            }
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
        if (displayData.bankDisplayMode != BankDisplayMode::OFF)
        {
          led24.setBar(0, LED_YELLOW);
        }
      }
      drawLed24Effects(bankIndex);
      led24.render();
    }
  }

  //------------------------------------------------------------------------------
  // Web

  // Replace page template tags with values
  virtual String Web_ParsePage(const String& var)
  {
    web.buffer.cls();

    // Bank count
    if (var == s_Web_BankCount)
    {
      web.buffer.print(bankCount);
    }
    else
    // Scene count
    if (var == s_Web_SceneCount)
    {
      web.buffer.print(SCENE_COUNT);
    }
    else
    // Scene default
    if (var == s_Web_SceneDefault)
    {
      web.buffer.print(bankSceneIndex);
    }
    else
    // Bank Mode count
    if (var == s_Web_BankModeCount)
    {
      web.buffer.print((int)BankDisplayMode::SIZE);
    }
    else
    // Bank Mode names
    if (var == s_Web_BankModeNames)
    {
      for (int i = 0; i < (int)BankDisplayMode::SIZE; ++i)
      {
        web.buffer.printf("'%s',", bankLabels[i].name);
      }
    }
    else
    // Bank Scenes
    if (var == s_Web_BankScenes)
    {
      web.buffer.cls();

      for (int iScene = 0; iScene < SCENE_COUNT; ++iScene)
      {
        for (int iBank = 0; iBank < bankCount; ++iBank)
        {
          web.buffer.printf("%d,", bankScenes[iScene].modes[iBank]);
        }
        web.buffer.println();
      }
    }
    // not recognized
    else
    {
      return String(var);
    }

    return String(web.buffer.c_str());
  }

  // Web Form processing
  virtual void Web_ProcessForm(AsyncWebServerRequest* request)
  {
    if (Web_ProcessForm_SceneDefault(request))
    {
      return;
    }

    if (Web_ProcessForm_BankMode(request))
    {
      return;
    }
  }

  // Scene Default
  virtual bool Web_ProcessForm_SceneDefault(AsyncWebServerRequest* request)
  {
    if (request->hasArg(s_Web_SceneDefaultRadio))
    {
      int value = request->arg(s_Web_SceneDefaultRadio).toInt();
      if (value == constrain(value, 0, SCENE_COUNT - 1))
      {
        if (bankSceneIndex != value)
        {
          bankSceneIndex = value;
          prefsStore();
        }
      }

      return true;
    }

    return false;
  }

  // Bank Display Mode
  virtual bool Web_ProcessForm_BankMode(AsyncWebServerRequest* request)
  {
    if (request->hasArg(s_Web_BankModeForm))
    {
      if (request->hasArg(s_Web_BankModeScene)
        && request->hasArg(s_Web_BankModeBank)
        && request->hasArg(s_Web_BankModeDropdown))
      {
        int iScene = request->arg(s_Web_BankModeScene).toInt();
        int iBank = request->arg(s_Web_BankModeBank).toInt();
        int iBankMode = request->arg(s_Web_BankModeDropdown).toInt();

        if (setBankDisplayMode(iScene, iBank, iBankMode))
        {
          prefsStore();
        }
      }

      return true;
    }

    return false;
  }
};

#endif// KSP_MeterQuad_h
