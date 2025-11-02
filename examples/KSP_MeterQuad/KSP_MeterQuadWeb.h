// (c) 2025 gogodyne
#ifndef KSP_MeterQuadWeb_h
#define KSP_MeterQuadWeb_h

#include <KontrolRack_ESPWiFi.h>

#define s_Web_StylePath       "/style.css"
#define s_Web_ScriptPath      "/script.js"
#define s_Web_IndexPath       "/index.html"

using namespace KontrolRack;

////////////////////////////////////////////////////////////////////////////////
class KSP_MeterQuadWeb
{
public:
  PrintBuffer<Web_BufferSize> buffer;// for web HTML response
  ESPWiFi net;

  virtual void begin()
  {
    net.begin();
    beginWeb();
  }

  virtual void loop()
  {
    net.loop();
  }

  virtual void beginWeb()
  {
    // FileNotFound
    net.server.onNotFound(
      [this]( AsyncWebServerRequest *request )
      { onFileNotFound( request ); }
    );

    // static pages
    net.serveStatic( s_Web_StylePath );
    net.serveStatic( s_Web_ScriptPath );

    // root/home page
    net.server.on( "/",        HTTP_GET,
      [this]( AsyncWebServerRequest *request )
      { request->send( SPIFFS, s_Web_IndexPath, "", false, [this](const String& var) { return onHomePage(var); } ); }
    );

    // form post
    net.server.on( "/",        HTTP_POST,
      [this]( AsyncWebServerRequest *request )
      {
        onForm( request,
          [this]( const String& argName, const String& argValue )
          { onFormPost( argName, argValue ); }
          );
      }
    );
  }

  virtual void onFileNotFound( AsyncWebServerRequest* request )
  {
    buffer.cls();
    buffer.print("File Not Found\n\n");
    buffer.print("URI: ");
    buffer.print(request->url().c_str());
    buffer.print("\nMethod: ");
    buffer.print(( request->method() == HTTP_GET ) ? "GET" : "POST");
    buffer.print("\nArguments: ");
    buffer.print(request->args());
    buffer.print("\n");
    for (uint8_t i = 0; i <request->args(); ++i)
    {
      buffer.print(" ");
      buffer.print(request->argName( i ).c_str());
      buffer.print(": ");
      buffer.print(request->arg( i ));
      buffer.print("\n");
    }
    request->send( 404, "text/plain", buffer.c_str() );
  }

  virtual String onHomePage(const String& var )
  {
    if ( var == "HOST_NAME" )     return net.hostName;
    if ( var == "APP_TITLE" )     return APP_TITLE;

    // if ( var == "CFG_INTERVAL_NAME" )       return Cfg::Interval::varName;
    // if ( var == "CFG_INTERVAL_MIN" )        return String( Interval_Min );
    // if ( var == "CFG_INTERVAL_TYPE_OPTIONS" )
    // {
    //   WorldData::buffer.cls();
    //   for ( int i = 0; i < Cfg::Interval::Type::count; ++i )
    //   {
    //     WorldData::buffer.printf( "\n<option value='%d'%s>%s</option>",
    //       i,
    //       ( WorldData::cfgData.intervalData.type == i ) ? " selected" : "",
    //       Cfg::Interval::Preset( (Cfg::Interval::Type)i ).name
    //       );
    //   }
    //   return String( WorldData::buffer.c_str() );
    // }

    return String();
  }

  void onForm( AsyncWebServerRequest* request, std::function<void( const String& argName, const String& argValue )> onPost )
  {
    if ( request->method() == HTTP_POST )
    {
      for ( int i = 0; i < request->args(); ++i )
        onPost( request->argName( i ), request->arg( i ) );
    }
    // continue...
    request->redirect( "/" );
  }


  void onFormPost( const String& argName, const String& argValue )
  {
    // int value;

    // // interval
    // if ( argName == Cfg::Interval::varName )
    // {
    //   value = argValue.toInt();
    //   if ( WorldData::cfgData.intervalData.value != value )
    //   {
    //     WorldData::cfgData.intervalData.value = Cfg::Interval::clamp( value );
    //     Storage_Store();
    //     Slide_Start();
    //   }
    // }
    // else
    // // advance command
    // if ( argName == s_Web_Advance )
    // {
    //   value = argValue.toInt();
    //   if ( value )
    //   {
    //     Slide_Advance( value == -1 );
    //   }
    // }
  }
};

#endif// KSP_MeterQuadWeb_h
