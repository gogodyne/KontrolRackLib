// (c) 2025 gogodyne
#ifndef KontrolRack_QRCode_h
#define KontrolRack_QRCode_h

#include <QRCodeGFX.h>

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
class QRCode
{
public:
  static void DrawWiFi(Adafruit_GFX& gfx, const char* hostName, int16_t x, int16_t y)
  {
    if (hostName && strlen(hostName))
    {
      QRCodeGFX qrcode(gfx);
      char text[48] = { 0 };

      snprintf(text, 48, "WIFI:S:%s;T:nopass;P:;H:blank;;", hostName);
      qrcode.setScale(2).setColors(SSD1306_BLACK, SSD1306_WHITE);
      qrcode.draw(text, x, y);
    }
  }
};

}// namespace KontrolRack

#endif// KontrolRack_QRCode_h
