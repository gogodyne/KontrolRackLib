// (c) 2025 gogodyne
#ifndef KontrolRack_Input_h
#define KontrolRack_Input_h

#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include <Wire.h>

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
// For tracking the phases of a button press.
class PressState
{
public:
  bool isPress : 1;// now pressed this frame
  bool wasPress : 1;// was pressed last frame
  bool didPress : 1;// changed to pressed this frame
  bool didRelease : 1;// changed to NOT pressed this frame
  bool didChange : 1;// changed from last frame

  virtual bool setPressState( bool newPress )
  {
    wasPress = isPress;
    isPress = newPress;
    didRelease = wasPress && !isPress;
    didPress = ( wasPress != isPress ) && isPress;
    didChange = wasPress != isPress;

    return didPress;
  }
};

////////////////////////////////////////////////////////////////////////////////
class ButtonState : public PressState
{
public:
  uint8_t pin;
  volatile bool pinState : 1;

  virtual void begin(uint8_t pin)
  {
    this->pin = pin;

    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), std::bind(&ButtonState::isrPin, this), CHANGE);
  }

  void IRAM_ATTR isrPin()
  {
    pinState = readPin();
  }

  virtual bool readPin()
  {
    return (digitalRead(pin) == LOW);// active LOW
  }

  virtual void loop()
  {
    setPressState(pinState);
  }
};

////////////////////////////////////////////////////////////////////////////////
class SliderState
{
public:
  int minPosition;
  int maxPosition;
  int position;
  bool isWrap : 1;

  virtual void set( int minPosition, int maxPosition, bool isWrap, int position )
  {
    this->minPosition = min( minPosition, maxPosition );
    this->maxPosition = max( minPosition, maxPosition );
    this->isWrap = isWrap;
    set( position );
  }

  virtual int set( int position )
  {
    return this->position = constrain( position, min( minPosition, maxPosition ), max( minPosition, maxPosition ) );
  }

  virtual int change( bool inc )
  {
    return inc ? increment() : decrement();
  }

  virtual int increment()
  {
    if ( isWrap && position == maxPosition )
      return position = minPosition;
    return set( position + 1 );
  }

  virtual int decrement()
  {
    if ( isWrap && position == minPosition )
      return position = maxPosition;
    return set(position - 1);
  }

  virtual float getLevel()
  {
    return (float)(position - minPosition) / (float)((maxPosition + 1) - minPosition);
  }
};

////////////////////////////////////////////////////////////////////////////////
#define ROTENC_DIR_NONE 0x0
#define ROTENC_DIR_CW 0x10
#define ROTENC_DIR_CCW 0x20
#define ROTENC_START 0x0
#define ROTENC_CW_FINAL 0x1
#define ROTENC_CW_BEGIN 0x2
#define ROTENC_CW_NEXT 0x3
#define ROTENC_CCW_BEGIN 0x4
#define ROTENC_CCW_FINAL 0x5
#define ROTENC_CCW_NEXT 0x6
#define ROTENC_GetStateDir(state) ((state) & 0x30)
#define ROTENC_GetStateDelta(state) (ROTENC_GetStateDir(state) == ROTENC_DIR_CW ? 1 : ROTENC_GetStateDir(state) == ROTENC_DIR_CCW ? -1 : 0)
////////////////////////////////////////////////////////////////////////////////
const unsigned char ROTENC_states[7][4] =
{
  // ROTENC_START
  { ROTENC_START,    ROTENC_CW_BEGIN,  ROTENC_CCW_BEGIN, ROTENC_START },
  // ROTENC_CW_FINAL
  { ROTENC_CW_NEXT,  ROTENC_START,     ROTENC_CW_FINAL,  ROTENC_START | ROTENC_DIR_CW },
  // ROTENC_CW_BEGIN
  { ROTENC_CW_NEXT,  ROTENC_CW_BEGIN,  ROTENC_START,     ROTENC_START },
  // ROTENC_CW_NEXT
  { ROTENC_CW_NEXT,  ROTENC_CW_BEGIN,  ROTENC_CW_FINAL,  ROTENC_START },
  // ROTENC_CCW_BEGIN
  { ROTENC_CCW_NEXT, ROTENC_START,     ROTENC_CCW_BEGIN, ROTENC_START },
  // ROTENC_CCW_FINAL
  { ROTENC_CCW_NEXT, ROTENC_CCW_FINAL, ROTENC_START,     ROTENC_START | ROTENC_DIR_CCW },
  // ROTENC_CCW_NEXT
  { ROTENC_CCW_NEXT, ROTENC_CCW_FINAL, ROTENC_CCW_BEGIN, ROTENC_START },
};
////////////////////////////////////////////////////////////////////////////////
class RotaryEncoder
{
public:
  volatile uint8_t pinState = 0b00;
  volatile uint8_t state = ROTENC_START;
  volatile int position = 0;
  volatile int8_t lastChange = 0;
  int lastPosition = 0;
  uint16_t positionCount = 20;

  // Pins
  uint8_t pinA = 0;// or Exp interrupt
  uint8_t pinB = 0;

  virtual void begin(uint16_t positionCount)
  {
    this->positionCount = positionCount;
  }

  virtual void begin(uint16_t positionCount, uint8_t pinA, uint8_t pinB)
  {
    begin(positionCount);

    this->pinA = pinA;
    this->pinB = pinB;

    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pinA), std::bind(&RotaryEncoder::isrPins, this), CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinB), std::bind(&RotaryEncoder::isrPins, this), CHANGE);
  }

  void IRAM_ATTR isrPins()
  {
    read();
  }

  virtual int8_t read()
  {
    pinState = (digitalRead(pinB) << 1) | digitalRead(pinA);
    state = ROTENC_states[state & 0xf][pinState];
    lastChange = ROTENC_GetStateDelta(state);
    position += lastChange;

    return lastChange;
  }

  virtual void loop()
  {}

  virtual int popChange()
  {
    int last = lastPosition;
    lastPosition = position;

    return position - last;
  }

  virtual float getRevolution()
  {
    return (float)(position % positionCount) / (float)(positionCount);
  }
};

////////////////////////////////////////////////////////////////////////////////
// Encoder with a Slider
class EncSlider : public RotaryEncoder
{
public:
  using RotaryEncoder::begin;
  SliderState slider;

  virtual void begin(uint16_t positionCount) override
  {
    RotaryEncoder::begin(positionCount);

    slider.set(0, positionCount - 1, true, 0);
  }

  virtual int8_t read() override
  {
    int8_t delta = RotaryEncoder::read();
    if (delta != 0)
    {
      slider.change(delta > 0);
    }

    return delta;
  }
};

////////////////////////////////////////////////////////////////////////////////
// Encoder/Slider with a Button
class EncBtn
{
public:
  ////////////////////////////////////////////////////////////////////////////////
  struct Info
  {
    uint16_t positionCount = 0;
    int8_t pinA = -1;
    int8_t pinB = -1;
    int8_t pinS = -1;

    Info() {}
    Info(uint16_t positionCount, int8_t pinA, int8_t pinB, int8_t pinS)
    : positionCount(positionCount)
    , pinA(pinA)
    , pinB(pinB)
    , pinS(pinS)
    {}
  };

  int encDelta;
  EncSlider enc;
  ButtonState btn;

  virtual void begin(Info info)
  {
    enc.begin(info.positionCount, info.pinA, info.pinB);
    btn.begin(info.pinS);
  }

  virtual void loop()
  {
    enc.loop();
    btn.loop();

    encDelta = enc.popChange();
  }

  virtual bool isPress() { return btn.isPress; }
  virtual bool wasPress() { return btn.wasPress; }
  virtual bool didPress() { return btn.didPress; }
  virtual bool didRelease() { return btn.didRelease; }
  virtual bool didChangeBtn() { return btn.didChange; }
  virtual bool didIncrease() { return encDelta > 0; }
  virtual bool didDecrease() { return encDelta < 0; }
  virtual bool didChangeEnc() { return didIncrease() || didDecrease(); }
  virtual bool didChange() { return didChangeBtn() || didChangeEnc(); }
};

}// namespace KontrolRack

#endif// KontrolRack_Input_H
