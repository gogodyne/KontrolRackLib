// (c) 2025 gogodyne
#ifndef KontrolRack_LEDButton_h
#define KontrolRack_LEDButton_h

#include <Adafruit_PCF8575.h>
#include <KontrolRack_Input.h>
#include <KontrolRack_Timing.h>

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
// 1 input switch and 1 output LED
class LEDButton : public PressState
{
public:
  enum OutputMode
  {
    Off     = 0,
    On      = 1,// solid
    Active  = 2,// blink
  };
  int outputMode = OutputMode::Off;
  bool outputState = false;
  volatile bool inputState = false;

  virtual void loop(bool isBlink)
  {
    setPressState(inputState);
    draw(isBlink);
  }

  virtual void setOutputMode(int outputMode)
  {
    this->outputMode = outputMode;
  }

  virtual void draw(bool isBlink)
  {
    drawOutput((outputMode == LEDButton::OutputMode::On) || ((outputMode == LEDButton::OutputMode::Active) && isBlink));
  }

  virtual void drawOutput(bool isOn, bool force = false)
  {
    outputState = isOn;
  }
};

////////////////////////////////////////////////////////////////////////////////
class LEDButton1 : public LEDButton
{
public:
  uint8_t pinInput = 0;
  uint8_t pinOutput = 0;

  virtual void begin(uint8_t pinInput, uint8_t pinOutput)
  {
    this->pinInput = pinInput;
    pinMode(pinInput, INPUT_PULLUP);
    attachInterrupt( digitalPinToInterrupt(pinInput), [this]() { inputState = (digitalRead(this->pinInput) == LOW); }, CHANGE );

    this->pinOutput = pinOutput;
    pinMode(pinOutput, OUTPUT);
  }

  virtual void drawOutput(bool isOn, bool force = false)
  {
    bool wasOn = outputState;
    outputState = isOn;
    if (force || outputState != wasOn)
    {
      digitalWrite(pinOutput, outputState ? LOW : HIGH);
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
class LEDButton10
{
public:
  Timing timing;

  Adafruit_PCF8575 mux16;
  uint16_t muxState = 0;
  LEDButton btn[8];
  LEDButton1 btn8;
  LEDButton1 btn9;

  virtual void begin(uint8_t btn8_pinIn, uint8_t btn8_pinOut, uint8_t btn9_pinIn, uint8_t btn9_pinOut, uint8_t i2c_addr = PCF8575_I2CADDR_DEFAULT, TwoWire *wire = &Wire)
  {
    timing.begin(30);
    mux16.begin(i2c_addr, wire);

    for (uint8_t i = 0; i < 8; ++i)
    {
      int ioIn = i * 2;
      int ioOut = ioIn + 1;

      mux16.pinMode(ioIn, INPUT_PULLUP);
      mux16.pinMode(ioOut, OUTPUT);
      mux16.digitalWrite(ioOut, HIGH);
    }

    btn8.begin(btn8_pinIn, btn8_pinOut);
    btn9.begin(btn9_pinIn, btn9_pinOut);
  }

  virtual void loop()
  {
    timing.loop();
    bool isBlink = (timing.isHz(2) || timing.isHz(4));

    muxState = mux16.digitalReadWord();
    for (uint8_t i = 0; i < 8; ++i)
    {
      int iIn = i * 2;
      int iOut = iIn + 1;
      bool isOn = ((muxState >> iIn) & 1) == 0;
      int pinValue = HIGH;
      if ((btn[i].outputMode == LEDButton::OutputMode::On) || ((btn[i].outputMode == LEDButton::OutputMode::Active) && isBlink))
      {
        pinValue = LOW;
      }

      // Input
      btn[i].setPressState(isOn);
      // Output
      mux16.digitalWrite(iOut, pinValue);
    }

    btn8.loop(isBlink);
    btn9.loop(isBlink);
  }

  virtual LEDButton& getButton(uint8_t index)
  {
    if ((index >= 0) && (index < 8))
    {
      return btn[index];
    }

    switch (index)
    {
    case 8:
      return btn8;
    case 9:
      return btn9;
    }

    return btn[0];
  }
};

}// namespace KontrolRack

#endif// KontrolRack_LEDButton_h
