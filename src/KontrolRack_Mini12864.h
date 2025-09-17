// (c) 2025 gogodyne
#ifndef KontrolRack_Mini12864_h
#define KontrolRack_Mini12864_h

#include "KontrolRack_Timing.h"
#include "KontrolRack_Input.h"

#define Mini12864_FPS 2// LCD has a long refresh
#define Mini12864_Contrast    0xA0
#define Mini12864_Brightness  0xFF

// Backlight/Buttonlight
#define Mini12864_NEOPIXEL_COUNT 3
#define Mini12864_NEOPIXEL_BACKLIGHT 0
#define Mini12864_NEOPIXEL_BTNLIGHTL 1
#define Mini12864_NEOPIXEL_BTNLIGHTR 2

// backlight
#include <Adafruit_NeoPixel.h>

// LCD
// https://github.com/olikraus/u8g2/wiki/u8g2reference
#include <U8g2lib.h>
#ifndef U8G2_16BIT
#error 16-bit is required; uncomment U8G2_16BIT in "u8g2.h"
#endif
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Mini12864 Orientation, relative to its rotary encoder.
#define Mini12864_LCDLeft     U8G2_R2
#define Mini12864_LCDRight    U8G2_R0
#define Mini12864_LCDAbove    U8G2_R1
#define Mini12864_LCDBelow    U8G2_R3
#define Mini12864_LCDMirrorH  U8G2_MIRROR
#define Mini12864_LCDMirrorV  U8G2_MIRROR_VERTICAL

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
// "Mini12864 V2.1"
// https://wiki.fysetc.com/docs/Mini12864Panel
class Mini12864
{
public:
  Timing timing;
  EncBtn encBtn;
  bool usingEncoder = false;
  Adafruit_NeoPixel backlight;
  U8G2_ST7567_JLX12864_1_4W_HW_SPI lcd;

  Mini12864(int lcdCS, int lcdRS, int lcdReset, int backlight, const u8g2_cb_t* orientation)
  : timing()
  , encBtn()
  , backlight(Mini12864_NEOPIXEL_COUNT, backlight, NEO_GRB)
  , lcd(orientation, lcdCS, lcdRS, lcdReset)
  {}

  virtual void begin(int positionCount, int encA, int encB, int encS)
  {
    timing.begin(Mini12864_FPS);

    if (encA > -1 && encB > -1 && encS > -1)
    {
      usingEncoder = true;
      encBtn.begin(positionCount, encA, encB, encS);
    }

    backlight.begin();
    backlight.setBrightness(Mini12864_Brightness);
    backlight.setPixelColor(Mini12864_NEOPIXEL_BACKLIGHT, backlight.Color(0xFF, 0x00, 0x00));
    backlight.setPixelColor(Mini12864_NEOPIXEL_BTNLIGHTL, backlight.Color(0x0F, 0x00, 0x00));
    backlight.setPixelColor(Mini12864_NEOPIXEL_BTNLIGHTR, backlight.Color(0x00, 0x00, 0x0F));
    backlight.show();

    lcd.begin();
    lcd.setContrast(Mini12864_Contrast);
  }

  virtual void loop(bool dirtyByEncoder = true)
  {
    timing.loop();
    encBtn.loop();

    bool isDirty = (dirtyByEncoder && (encBtn.btn.didChange || encBtn.encDelta));
    if (timing.isTick || isDirty)
    {
      render();
    }
  }

  virtual void render()
  {
    lcd.firstPage(); do
    {
      draw("TEST");
    }
    while (lcd.nextPage());
  }

  virtual void draw(const char* text)
  {
    int width = lcd.getWidth();
    int height = lcd.getHeight();

    int value = encBtn.enc.slider.position;
    float level = encBtn.enc.slider.getLevel();
    int flags = U8G2_BTN_BW1 | U8G2_BTN_HCENTER | (encBtn.btn.isPress ? U8G2_BTN_INV : 0);

    lcd.setFont(u8g2_font_6x12_tr);
    lcd.setFontMode(0);

    lcd.drawButtonUTF8(width/2, 10, flags, width-4, 1, 1, text);

    lcd.setCursor(0, 24);
    lcd.print(millis()/1000);
    lcd.setCursor(0, 34);
    lcd.print(value);

    lcd.drawFrame(0, height-4, width, 4);
    lcd.drawBox(0, height-4, width * level, 4);
  }
};

}// namespace KontrolRack

#endif// KontrolRack_Mini12864_h
