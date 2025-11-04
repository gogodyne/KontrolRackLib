// (c) 2025 gogodyne
#ifndef KontrolRack_QRCode_h
#define KontrolRack_QRCode_h

#include <QRCodeGFX.h>

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
class QRCode
{
public:
  static void drawWiFi(Adafruit_GFX& gfx, const char* ssid, int16_t x, int16_t y)
  {
    if (ssid && strlen(ssid))
    {
      QRCodeGFX qrcode(gfx);
      char text[48] = { 0 };
      snprintf(text, 48, "WIFI:S:%s;T:nopass;P:;H:blank;;", ssid);

      qrcode.setScale(2).setColors(SSD1306_BLACK, SSD1306_WHITE);
      qrcode.draw(text, x, y);
    }
  }

  static void drawURL(Adafruit_GFX& gfx, const char* ip, int16_t x, int16_t y)
  {
    if (ip && strlen(ip))
    {
      QRCodeGFX qrcode(gfx);
      char text[48] = { 0 };
      snprintf(text, 48, "http://%s", ip);

      qrcode.setScale(2).setColors(SSD1306_BLACK, SSD1306_WHITE);
      qrcode.draw(text, x, y);
    }
  }
};

}// namespace KontrolRack

#endif// KontrolRack_QRCode_h
