// (c) 2025 gogodyne
#ifndef KontrolRack_MAX7219_h
#define KontrolRack_MAX7219_h

#define MAX7219_NOOP                    0x00
//      MAX7219_Digit0-7                0x01-0x08
#define MAX7219_DecodeMode              0x09
#define MAX7219_DecodeMode_NoDecode     0x00// No decode for digits 7–0
#define MAX7219_DecodeMode_CodeBfor0    0x01// Code B decode for digit 0, No decode for digits 7–1
#define MAX7219_DecodeMode_CodeBfor3    0x0F// Code B decode for digits 3–0, No decode for digits 7–4
#define MAX7219_DecodeMode_CodeBAll     0xFF// Code B decode for digits 7–0
#define MAX7219_Intensity               0x0A// 0x00-0x0F
#define MAX7219_Intensity_Min           0x00
#define MAX7219_Intensity_Max           0x0F
#define MAX7219_ScanLimit               0x0B// 0x00-0x07 digits
#define MAX7219_ShutdownMode            0x0C
#define MAX7219_ShutdownMode_Shutdown   0x00
#define MAX7219_ShutdownMode_Normal     0x01
#define MAX7219_DisplayTestMode         0x0F
#define MAX7219_DisplayTestMode_Normal  0x00
#define MAX7219_DisplayTestMode_Test    0x01

#define MAX7219_DigitCount  8// maximum digits supported
#define MAX7219_DeviceMax   8// maximum displays supported
#define MAX7219_BufferSize (MAX7219_DigitCount * MAX7219_DeviceMax)

#define CodeB_Minus   (uint8_t)0x0A
#define CodeB_E       (uint8_t)0x0B
#define CodeB_H       (uint8_t)0x0C
#define CodeB_L       (uint8_t)0x0D
#define CodeB_P       (uint8_t)0x0E
#define CodeB_Space   (uint8_t)0x0F
#define CodeB_DP      (uint8_t)0x80

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
// 8-digit 7-segment numeric display.
// Up to 8 units.
// Driver: MAX7219
class Num8
{
public:
  ////////////////////////////////////////////////////////////////////////////////
  struct Info
  {
    int8_t din = -1;
    int8_t cs = -1;
    int8_t clk = -1;
    float intensity = 1.f;
    Info() {}
    Info(int8_t din, int8_t cs, int8_t clk, float intensity)
    : din(din)
    , cs(cs)
    , clk(clk)
    , intensity(intensity)
    {}
  };

  int din;
  int cs;
  int clk;
  uint8_t buffer[MAX7219_DeviceMax][MAX7219_DigitCount];
  uint8_t intensities[MAX7219_DeviceMax];

  void begin(int din, int cs, int clk, float intensity)
  {
    this->din = din;
    this->cs = cs;
    this->clk = clk;

    pinMode(din, OUTPUT);
    pinMode(cs, OUTPUT);
    pinMode(clk, OUTPUT);
    digitalWrite(clk, HIGH);
    delay(200);

    setDisplayTestMode(MAX7219_DisplayTestMode_Normal);
    setShutdownMode(MAX7219_ShutdownMode_Normal);
    setScanLimit(0x07);
    setIntensity(intensity);
    setDecodeMode(MAX7219_DecodeMode_CodeBAll);
  }

  void begin (Info info)
  {
    begin(info.din, info.cs, info.clk, info.intensity);
  }

  ////////////////////////////////////////////////////////////////////////////////

  inline void open()    { digitalWrite(cs, LOW); }
  inline void close()   { digitalWrite(cs, HIGH); }

  void send(uint8_t reg, byte data)
  {
    shiftOut(din, clk, MSBFIRST, reg);
    shiftOut(din, clk, MSBFIRST, data);
  }

  ////////////////////////////////////////////////////////////////////////////////

  virtual void clear()
  {
    // clear the buffer
    for (int unit = 0; unit < MAX7219_DeviceMax; ++unit)
      for (int col = 0; col < MAX7219_DigitCount; ++col)
        buffer[unit][col] = CodeB_Space;
  }

  virtual void render()
  {
    // set intensities
    open();
    for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
      send(MAX7219_Intensity, intensities[unit]);
    close();

    // render digits
    for (int col = 0; col < MAX7219_DigitCount; ++col)
    {
      open();
      for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
        send(col + 1, buffer[unit][col]);
      close();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////

  virtual void setDisplayTestMode(uint8_t value)
  {
    open();
    for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
      send(MAX7219_DisplayTestMode, value);
    close();
  }

  virtual void setShutdownMode(uint8_t value)
  {
    open();
    for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
      send(MAX7219_ShutdownMode, value);
    close();
  }

  virtual void setScanLimit(uint8_t value)
  {
    open();
    for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
      send(MAX7219_ScanLimit, value);
    close();
  }

  virtual void setIntensity(float amount)
  {
    for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
      setIntensity(unit, amount);
  }

  virtual void setIntensity(uint8_t unit, float amount)
  {
    amount = constrain(amount, 0.f, 1.f);
    amount *= amount;// brightness is logarithmic
    uint8_t value = amount * (float)MAX7219_Intensity_Max;

    intensities[unit] = value;
  }

  virtual void setIntensity(uint8_t value)
  {
    for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
      setIntensity(unit, value);
  }

  virtual void setIntensity(uint8_t unit, uint8_t value)
  {
    intensities[unit] = value;
  }

  virtual void setDecodeMode(uint8_t value)
  {
    open();
    for (int unit = MAX7219_DeviceMax - 1; unit > -1; --unit)
      send(MAX7219_DecodeMode, value);
    close();
  }

  virtual void test(int value)
  {
#if 0
    for (int unit = 0; unit < MAX7219_DeviceMax; ++unit)
      printUnit(unit, value);
#else
    unsigned long ms = millis();
    printUnit(0, "7654.3210");
    printUnit(1, ms / 1000., 3);
    printUnit(2, ms);
    // printUnit(3, 32768);
    printUnit(3, (long)32*1024);
    printUnit(4, value);
#endif
  }

  virtual void printUnit(int unit, unsigned long value)
  {
    printUnit(unit, (double)value);
  }

  virtual void printUnit(int unit, long value)
  {
    printUnit(unit, (double)value);
  }

  virtual void printUnit(int unit, int value)
  {
    printUnit(unit, (double)value);
  }

  void printUnit(int unit, double value, unsigned char prec = 0)
  {
    char buf[32];
    dtostrf(value, 0, prec, buf);
    printUnit(unit, buf);
  }

  virtual void printUnit(int unit, const char* s)
  {
    if (s)
    {
      int len = strlen(s);
      if (len > 0)
      {
        int col = 0;

        // clear the unit
        for (int i = 0; i < MAX7219_DigitCount; ++i)
        {
          buffer[unit][i] = CodeB_Space;
        }

        // find number characters + space
        for (int i = 0; i < len && col < MAX7219_DigitCount; ++i)
        {
          uint8_t c = s[len - 1 - i];
          if (c >= '0' && c <= '9')
          {
            buffer[unit][col] = (c - '0') | (buffer[unit][col] & CodeB_DP ? CodeB_DP : 0);
            ++col;
          }
          else
          // numeral
          if (c == '.' || c == ',')
          {
            if (buffer[unit][col] & CodeB_DP)
            {
              buffer[unit][col] = CodeB_Space | CodeB_DP;
              ++col;
            }
            if (col < MAX7219_DigitCount)
              buffer[unit][col] |= CodeB_DP;
          }
          else
          // sign
          if (c == '-')
          {
            buffer[unit][col] = CodeB_Minus | (buffer[unit][col] & CodeB_DP ? CodeB_DP : 0);
            ++col;
          }
          else
          // exponent
          if (c == 'E' || c == 'e')
          {
            buffer[unit][col] = CodeB_E | (buffer[unit][col] & CodeB_DP ? CodeB_DP : 0);
            ++col;
          }
          else
          // space
          if (c == ' ')
          {
            buffer[unit][col] = CodeB_Space | (buffer[unit][col] & CodeB_DP ? CodeB_DP : 0);
            ++col;
          }
        }
      }
    }
  }
};

}// namespace KontrolRack

#endif// KontrolRack_DigMAX7219_h
