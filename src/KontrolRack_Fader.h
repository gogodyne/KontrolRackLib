// (c) 2025 gogodyne
#ifndef KontrolRack_Fader_h
#define KontrolRack_Fader_h

#define Fader_AnalogReadResolution 10
#define Fader_MakeChaseThreshold(max) ((float)(max)*.05)

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
// - fader/potentiometer; 1P INPUT
class Fader
{
public:
  int faderLo = 0;
  int faderHi = 1023;
  int faderValue = 0;
  float faderLevel = 0;
  uint8_t ADCResolution = Fader_AnalogReadResolution;// in bits
  int ADCMax = 1023;// ==1.==100%
  int chaseThreshold = 64;

  int pinFader;


  virtual void setup(int pinFader, int faderLo, int faderHi, uint8_t ADCResolution = Fader_AnalogReadResolution)
  {
    this->pinFader = pinFader;
    this->faderLo = faderLo;
    this->faderHi = faderHi;
    pinMode(pinFader, INPUT_PULLUP);
    this->ADCResolution = ADCResolution;
    setAnalogReadResolution(ADCResolution);
    ADCMax = pow(2, ADCResolution) - 1;
    chaseThreshold = Fader_MakeChaseThreshold(ADCMax);
  }

  virtual void loop()
  {
    faderValue = analogRead(pinFader);
    faderLevel = makeFaderLevel();
  }

  virtual float makeFaderLevel()
  {
    return std::clamp((float)(faderValue - faderLo) / (float)(faderHi - faderLo), 0.f, 1.f);
  }

  virtual void setAnalogReadResolution(uint8_t resolution)
  {
#if defined(ARDUINO_ARCH_ESP32)
    analogReadResolution(resolution);
#endif
  }
};

////////////////////////////////////////////////////////////////////////////////
// - touch terminal; 1P INPUT touch
// - motor +/- signals (to driver); 2P OUTPUT: H/L = fwd, L/H = rev, L/L = off
// To trigger the motor, do one of:
// - set the level
// - set the target and set chase
class FaderMotorized : public Fader
{
public:
  enum Direction { Stop=0, Forward=1, Reverse=-1 };

  int chaseTarget = 0;
  int chaseThreshold = 50;
  int chaseOffset = 0;
  bool chase = false;
  Direction motorDir = Direction::Stop;
  int touchValue = 0;
  int touchThreshold = 20;
  int touchLo = 22600;
  int touchHi = 22700;
  bool isTouched = false;

  struct MotorPins
  {
    int motorStby = -1;
    int motor1 = -1;
    int motor2 = -1;
    int touch = -1;
  };
  MotorPins motorPins;


  virtual void setup(int pinFader, int faderLo, int faderHi, int pinMotorStby, int pinMotor1, int pinMotor2, int pinTouch, int chaseThreshold, int touchThresholdLo, int touchThresholdHi)
  {
    Fader::setup(pinFader, faderLo, faderHi);

    motorPins.motorStby = pinMotorStby;
    if (motorPins.motorStby > -1)
    pinMode(motorPins.motorStby, OUTPUT);

    motorPins.motor1 = pinMotor1;
    motorPins.motor2 = pinMotor2;
    pinMode(motorPins.motor1, OUTPUT);
    pinMode(motorPins.motor2, OUTPUT);

    motorPins.touch = pinTouch;
    pinMode(pinTouch, INPUT_PULLUP);

    this->chaseThreshold = chaseThreshold;
    setTouchThreshold(touchThresholdLo, touchThresholdHi);

    stopMotor();
  }

  virtual void loop()
  {
    Fader::loop();

    chaseOffset = chaseTarget - faderValue;
    touchValue = getTouchRead();
    touchThreshold = makeTouchThreshold();
    // isTouched = touchValue < touchThreshold;
    // Lolin S2 mini (ESP32S2) touch is HIGHER
    isTouched = touchValue > touchThreshold;

    chaseFader();
  }

  ////////////////////////////////////////////////////////////////////////////////

  virtual void setTouchThreshold(int lo, int hi)
  {
    touchLo = lo;
    touchHi = hi;
  }

  virtual float makeTouchThreshold()
  {
    // Lolin S2 mini (ESP32S2) touch is HIGHER
    return std::lerp((float)touchLo, (float)touchHi, 1.f - faderLevel);
  }

  virtual int getTouchRead()
  {
#if defined(ARDUINO_ARCH_ESP32)
    return touchRead(motorPins.touch);
#else
    return 0;
#endif
  }

  virtual void setChase(bool chase)
  {
    this->chase = chase;
  }

  virtual void chaseFader()
  {
    if (chaseOffset > chaseThreshold)
      motorDir = Direction::Forward;
    else
    if (chaseOffset < -chaseThreshold)
      motorDir = Direction::Reverse;
    else
      motorDir = Direction::Stop;

    setMotor((chase && !isTouched) ? motorDir : Direction::Stop);
    if (isTouched)
    {
      setLevel(faderLevel);
    }
  }

  virtual void setTarget(int chaseTarget)
  {
    this->chaseTarget = chaseTarget;
  }

  virtual void setLevel(float level, bool chaseLevel = true)
  {
    chaseTarget = fabs(level) * (float)ADCMax;
    setChase(chaseLevel);
  }

  virtual void setMotor(Direction dir)
  {
    digitalWrite(motorPins.motor1, dir ? (dir > 0 ? HIGH : LOW) : LOW);
    digitalWrite(motorPins.motor2, dir ? (dir < 0 ? HIGH : LOW) : LOW);

    if (motorPins.motorStby > -1)
    {
      digitalWrite(motorPins.motorStby, (dir == Direction::Stop) ? LOW : HIGH);
    }
  }

  virtual inline void stopMotor()
  {
    setMotor(Direction::Stop);
  }
};

}// namespace KontrolRack

#endif// KontrolRack_Fader_h
