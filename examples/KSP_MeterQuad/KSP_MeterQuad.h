// (c) 2025 gogodyne
#ifndef KSP_MeterQuad_h
#define KSP_MeterQuad_h

#include <KerbalSimpit.h>
KerbalSimpit mySimpit(Serial);  // Declare a KerbalSimpit object that will communicate using the "Serial" device.
#include <KontrolRack.h>
#include <KontrolRack_KR_LED24OLED12864.h>

using namespace KontrolRack;

////////////////////////////////////////////////////////////////////////////////
class KSP_MeterQuad : public KR::Meter24Quad
{
public:
  using Parent = KR::Meter24Quad;

  enum BankScene
  {
    Vessel,
    Stage,
    XenonGasVessel,
    XenonGasStage,
    MAX
  };

  // Data for display.
  struct DisplayData
  {
    const char* label = nullptr;
    const resourceMessage* resourceMsg = nullptr;
    float level = 0;
    uint8_t meterValue = 0;
    uint8_t percent = 0;
    float total = 0;
    float available = 0;
    bool isStage = false;

    DisplayData() {}
    DisplayData(const char* label, const resourceMessage& message, bool isStage)
    : label(label)
    , resourceMsg(&message)
    , isStage(isStage)
    {
      if (resourceMsg)
      {
        if (resourceMsg->total)
        {
          total = resourceMsg->total;
          available = resourceMsg->available;
          level = available / total;
          percent = level * 100;
          meterValue = level * ((float)LED24::getSize() - 1);
        }
      }
    }
  };

  // KSP Messages.
  void (*mySimpitHandler)(byte messageType, byte msg[], byte msgSize) = nullptr;
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

  flightStatusMessage flightStatusMsg;

  // Connection.
  int connectionState = 0;
  unsigned long pingMs = 0;

  // Bank.
  int bankScene = BankScene::Vessel;
  enum BankMode
  {
    Lf,
    Ox,
    Sf,
    Xe,
    Mp,
    Ev,
    SIZE
  };
  const char* bankLabels[BankMode::SIZE] =
  {
    "Lf",
    "Ox",
    "Sf",
    "Xe",
    "Mp",
    "Ev"
  };

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
    banks[0].mode = BankMode::Lf;
    banks[1].mode = BankMode::Ox;
    banks[2].mode = BankMode::Sf;
    banks[3].mode = BankMode::Mp;

    Parent::begin(12, false, SWITCH_ADDRESS_METER, OLED12864_ADDRESS, LED24::Info(LED24_ADDRESS, &wire, LED24_Brightness), EncBtn::Info(ROTENC_PositionCount, ROTENC_A, ROTENC_B, ROTENC_S));
  }

  virtual void loop() override
  {
    Parent::loop();

    if (btnDidPress())
    {
      ESP.restart();
    }
    if (encDidIncrease())
    {
      bankScene = min(bankScene + 1, BankScene::MAX - 1);
    }
    if (encDidDecrease())
    {
      bankScene = max(bankScene - 1, 0);
    }

    tryConnect();
    mySimpit.update();
  }

  //------------------------------------------------------------------------------
  // Connection

  virtual int tryConnect()
  {
    unsigned long ms = millis();

    // Ping.
    if (ms >= pingMs)
    {
      pingMs = ms + 3000;
      mySimpit.send(ECHO_REQ_MESSAGE, "PING\n", 6);
    }

    // Connect.
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

    case FLIGHT_STATUS_MESSAGE:
      {
        if (msgSize == sizeof(flightStatusMessage))
        {
          flightStatusMsg = parseMessage<flightStatusMessage>(msg);
        }
      }
      break;
    }
  }

  virtual DisplayData makeDisplayData(uint8_t bankIndex)
  {
    if (bankIndex == 0)
    {
      if (!flightStatusMsg.isInEVA())
      {
        if (bankScene == BankScene::Stage || bankScene == BankScene::XenonGasStage)
          return DisplayData(bankLabels[BankMode::Lf], liquidFuelStageMsg, true);
        return DisplayData(bankLabels[BankMode::Lf], liquidFuelMsg, false);
      }
    }

    if (bankIndex == 1)
    {
      if (!flightStatusMsg.isInEVA())
      {
        if (bankScene == BankScene::Stage || bankScene == BankScene::XenonGasStage)
          return DisplayData(bankLabels[BankMode::Ox], oxidizerStageMsg, true);
        return DisplayData(bankLabels[BankMode::Ox], oxidizerMsg, false);
      }
    }

    if (bankIndex == 2)
    {
      if (!flightStatusMsg.isInEVA())
      {
        if (bankScene == BankScene::Stage)
          return DisplayData(bankLabels[BankMode::Sf], solidFuelStageMsg, true);
        if (bankScene == BankScene::Vessel)
          return DisplayData(bankLabels[BankMode::Sf], solidFuelMsg, false);
        if (bankScene == BankScene::XenonGasStage)
          return DisplayData(bankLabels[BankMode::Xe], xenonGasStageMsg, true);
        if (bankScene == BankScene::XenonGasVessel)
          return DisplayData(bankLabels[BankMode::Xe], xenonGasMsg, false);
      }
    }

    if (bankIndex == 3)
    {
      if (!flightStatusMsg.isInEVA())
      {
        return DisplayData(bankLabels[BankMode::Mp], monopropellantMsg, false);
      }
      return DisplayData(bankLabels[BankMode::Ev], evaMonopropellantMsg, false);
    }

    return DisplayData();
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

    // OLED
    if (isDirty || oled12864Devices[bankIndex].timing.isTick)
    {
      oled12864.clear();
      drawBankInverted(bankIndex, oled12864Devices[bankIndex].timing.isHz(1.f/30.f));
      {
        if (displayData.label)
        {
          int16_t x1, y1;
          uint16_t w, h;

          // Label size.
          oled12864.gfx.setTextSize(LABELSIZE);
          oled12864.gfx.getTextBounds(displayData.label, 0, 0, &x1, &y1, &w, &h);

          // Label frame.
          uint16_t labelHeight = LABELSIZE + h + LABELSIZE;
          oled12864.gfx.drawRoundRect(1, 1, oled12864.gfx.width() - 2, labelHeight - 2, LABELSIZE + LABELSIZE, WHITE);

          // Label.
          oled12864.gfx.setTextSize(LABELSIZE);
          oled12864.gfx.setCursor((oled12864.gfx.width() - w) / 2, LABELSIZE);
          oled12864.gfx.print(displayData.label);

          // Indicators.
          oled12864.gfx.setTextSize(LABELSIZE_SM);
          oled12864.gfx.setCursor(LABELSIZE, LABELSIZE);
          {
            // Bank Scene.
            oled12864.gfx.printf("%d", bankScene);
            // Connection status.
            if (connectionState < 1)
            {
              oled12864.gfx.print((connectionState < 0) ? "*" : timing.isHz(2) ? "/" : "\\");
            }
            oled12864.gfx.println();
            oled12864.gfx.setCursor(oled12864.gfx.getCursorX() + LABELSIZE, oled12864.gfx.getCursorY());
            // Stage.
            if (displayData.isStage)
            {
              oled12864.gfx.print((timing.isHz(1) || timing.isHz(.5)) ? "STG" : "---");
            }
          }

          // Available/Total.
          char text[32];
          text[0] = 0;
          oled12864.gfx.setTextSize(LABELSIZE_SM);
          oled12864.gfx.setCursor(LABELSIZE, labelHeight + LABELSIZE);
          if (timing.isHz(1.f/3.f))
          {
            oled12864.gfx.print("+");
            sprintf(text, "%.1f", displayData.available);
          }
          else
          {
            oled12864.gfx.print("/");
            sprintf(text, "%.1f", displayData.total);
          }
          oled12864.gfx.setTextSize(LABELSIZE_SM);
          oled12864.gfx.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
          oled12864.gfx.setCursor(oled12864.gfx.width() - w - LABELSIZE, labelHeight + LABELSIZE);
          oled12864.gfx.print(text);
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
        if (displayData.resourceMsg)
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
      }
      led24.render();
    }
  }
};

#endif// KSP_MeterQuad_h
