// (c) 2025 gogodyne
#ifndef KSP_NumericQuadWeb_h
#define KSP_NumericQuadWeb_h

#include <KontrolRack_ESPWiFi.h>
#include <KontrolRack_QRCode.h>

#define s_Web_StylePath       "/style.css"
#define s_Web_IndexPath       "/index.html"

#define s_Web_BankCount         "BANK_COUNT"
#define s_Web_SceneCount        "SCENE_COUNT"
#define s_Web_SceneDefault      "SCENE_DEFAULT"
#define s_Web_BankModeCount     "BANK_MODECOUNT"
#define s_Web_BankModeNames     "BANK_MODENAMES"
#define s_Web_BankScenes        "BANK_SCENES"
#define s_Web_SceneGrid         "SCENE_GRID"
#define s_Web_BankModeForm      "bankModeForm"
#define s_Web_BankModeDropdown  "bankModeDropdown"
#define s_Web_BankModeScene     "bankModeScene"
#define s_Web_BankModeBank      "bankModeBank"
#define s_Web_SceneDefaultForm  "sceneDefaultForm"
#define s_Web_SceneDefaultRadio "sceneDefaultRadio"

using namespace KontrolRack;

typedef std::function<void(AsyncWebServerRequest* request)> FormProcessor;

////////////////////////////////////////////////////////////////////////////////
class KSP_NumericQuadWeb
{
public:
  PrintBuffer<Web_BufferSize> buffer;// for web HTML response
  ESPWiFi net;

  AwsTemplateProcessor onParsePage;
  FormProcessor onProcessForm;

  virtual void begin(AwsTemplateProcessor onParsePage, FormProcessor onProcessForm)
  {
    this->onParsePage = onParsePage;
    this->onProcessForm = onProcessForm;

    net.begin();
    beginWeb();
  }

  virtual void loop()
  {
    net.loop();
  }

  //------------------------------------------------------------------------------

  virtual void beginWeb()
  {
    // FileNotFound
    net.server.onNotFound(
      [this](AsyncWebServerRequest *request)
      { HandleFileNotFound(request); }
    );

    // static pages
    // net.serveStatic(s_Web_StylePath);
    // net.serveStatic(s_Web_ScriptPath);

    // style sheet
    net.server.on(s_Web_StylePath, HTTP_GET,
      [this](AsyncWebServerRequest *request)
      { request->send(SPIFFS, s_Web_StylePath, "", false, [this](const String& var) { return ParsePage(var); }); }
    );

    // root/home page
    net.server.on("/", HTTP_GET,
      [this](AsyncWebServerRequest *request)
      { request->send(SPIFFS, s_Web_IndexPath, "", false, [this](const String& var) { return ParsePage(var); }); }
    );

    // form post
    net.server.on("/", HTTP_POST,
      [this](AsyncWebServerRequest *request)
      { ProcessForm(request); }
    );
  }

  virtual void HandleFileNotFound(AsyncWebServerRequest* request)
  {
    buffer.cls();
    buffer.print("File Not Found\n\n");
    buffer.print("URI: ");
    buffer.print(request->url().c_str());
    buffer.print("\nMethod: ");
    buffer.print((request->method() == HTTP_GET) ? "GET" : "POST");
    buffer.print("\nArguments: ");
    buffer.print(request->args());
    buffer.print("\n");
    for (uint8_t i = 0; i <request->args(); ++i)
    {
      buffer.print(" ");
      buffer.print(request->argName(i).c_str());
      buffer.print(": ");
      buffer.print(request->arg(i));
      buffer.print("\n");
    }
    request->send(404, "text/plain", buffer.c_str());
  }

  virtual String ParsePage(const String& var)
  {
    if (var == "HOST_NAME")
    {
      return net.hostName;
    }

    if (var == "APP_TITLE")
    {
      return APP_TITLE;
    }

    if (onParsePage)
    {
      return onParsePage(var);
    }

    return var;
  }

  void ProcessForm(AsyncWebServerRequest* request)
  {
    if (request->method() == HTTP_POST)
    {
      if (onProcessForm)
      {
        onProcessForm(request);
      }
    }
    // continue...
    request->redirect("/");
  }

  //------------------------------------------------------------------------------
  // QR code

  virtual void drawQRCode_SSID(Adafruit_GFX& gfx, int16_t x, int16_t y)
  {
    QRCode::drawWiFi(gfx, net.hostName, x, y);
  }

  virtual void drawQRCode_URL(Adafruit_GFX& gfx, int16_t x, int16_t y)
  {
    QRCode::drawURL(gfx, WiFi.softAPIP().toString().c_str(), x, y);
  }
};

#endif// KSP_NumericQuadWeb_h
