// (c) 2025 gogodyne
#ifndef KontrolRack_ESPWiFi_h
#define KontrolRack_ESPWiFi_h
#include <SD.h>
#include <SPIFFS.h> // ESP32 file system
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wps.h>// WPS WiFi push-button connect
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>// https://github.com/me-no-dev/ESPAsyncWebServer
#include <esp_mac.h>

#define D_print(s) Serial.print(s);
#define D_println(s) Serial.println(s);

////////////////////////////////////////////////////////////////////////////////
// Network
#define App_Title "KontrolRack"
#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "GOGODYNE"
#define ESP_MODEL_NUMBER  "KONTROLRACK"
#define ESP_MODEL_NAME    "KONTROLRACK"

////////////////////////////////////////////////////////////////////////////////
#define Web_BufferSize (8 *1024)

#define Network_UdpLocalPort 3333
#define Network_PacketSize 256
#define Network_SendInterval 100

#define WiFi_Channel 1
#define WiFi_MaxConnections 1

#define WiFi_MacNameLength (6*2+1)
#define WiFi_MacNameNick (4*2)// third quad

#define WiFi_TimeoutSaved (4 *1000)
#define WiFi_TimeoutSD    (30 *1000)
#define WiFi_TimeoutWPS   (3*60 *1000)

#define SSID_PASS_FileName "/wifi.txt"

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
enum class WiFi_Msg
{
  Off,
  Connecting,
  STAMode,
  PressWPS,
  SSIDPassword,
  WPSOk,
  WPSFailRetry,
  WPSTimeoutRetry,
  APMode,
  APStart,
  APStop,
  // APSTAConnected,
  // APSTADisconnected,
  WiFiOk,
  Reconnect,
  Timeout,
};
const char* WiFi_Msgs[] = 
{
  "",
  "WiFi connect...",
  "STA started",
  "Press router's WPS...",
  "SSID,PASSWORD",
  "WPS Ok",
  "WPS fail retry...",
  "WPS timeout retry...",
  "AP mode...",
  "AP started",
  "AP stopped",
  // "AP STA connected",
  // "AP STA disconnected",
  "WiFi connected",
  "Reconnect...",
  "Timeout.",
};
#define S_WiFi_Msg(i) (WiFi_Msgs[(int)(i)])

////////////////////////////////////////////////////////////////////////////////
// ESP WPS config
static  esp_wps_config_t wpsConfig;
static WiFiEvent_t wifiEvent;
static const char* wifiMsg;

////////////////////////////////////////////////////////////////////////////////
class ESPWiFi
{
public:
  enum struct WiFiPhase
  {
    Off,
    Connected,
    SD,
    Saved,
    WPS,
    AP
  };

  WiFiPhase phase;
  unsigned long phaseTimeout;
  char macName[WiFi_MacNameLength];
  char hostName[(32)];
  char wpsSSID[(32)];
  AsyncWebServer server;
  bool isServer;
  bool isOn;
  bool isBusy;
  bool isAP;

  const char* macNick() { return macName + WiFi_MacNameLength - 4 - 1; }// last 4 digits, minus null

  ESPWiFi()
  : phase(WiFiPhase::Off)
  , server(80)
  {}

  virtual void begin()
  {
    // Init
    // wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    // esp_wifi_init(&wifi_config);

    // MAC as text
    byte mac[6] = {0};
    // WiFi.macAddress(mac);//2A51
    esp_efuse_mac_get_default(mac);// same as STA

    sprintf(macName, "%2.X%2.X%2.X%2.X%2.X%2.X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // host name
    sprintf(hostName, "%s%s", App_Title, macName + WiFi_MacNameNick);
    WiFi.setHostname(hostName);

    // events
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) { WiFi_OnEvent(event, info); });

    // WPS
    WiFi_WPSInitConfig(hostName);
    WiFi_WPSLoadSSID();
  }

  virtual void loop()
  {
    WiFi_loop();
    Server_loop();
  }

  //------------------------------------------------------------------------------
  // Utility

  virtual void printStatus(Print& printer)
  {
    if (isAP || WiFi.SSID().length())
    {
      printer.print("http://");
      printer.print(WiFi_IP());
      if (wpsSSID[0] == 0)
        strcpy(wpsSSID, isAP ? hostName : WiFi.SSID().c_str());
    }
    else
    {
      if (wifiMsg && wifiMsg[0])
        printer.print(wifiMsg);
    }
  }

  //------------------------------------------------------------------------------
  // Server

  virtual void Server_loop()
  {
    // start
    if (!isServer && (WiFi.SSID().length() || isAP))
    {
      server.begin();
      isServer = true;
    }
    // running
    if (isServer)
    {
      // connected
      if (WiFi_IsConnected())
      {
        // nothing
      }
      else
      // disconnected; stop
      {
        server.end();
        isServer = false;
      }
    }
  }

  virtual void serveStatic(const char* uri, const char* path = NULL)
  {
    server.serveStatic(uri, SPIFFS, path ? path : uri);
  }

  //------------------------------------------------------------------------------
  // WiFi

  virtual bool WiFi_IsBusy()
  {
    return (isOn && !isServer && isBusy);
  }

  virtual bool WiFi_IsConnected()
  {
    return isAP || strlen(WiFi.SSID().c_str()) > 0;
  }

  virtual void WiFi_Disconnect(bool forget)
  {
    if (isAP)
      WiFi.softAPdisconnect();
    else
    {
      WiFi.mode(WIFI_STA);
      WiFi.disconnect(true, forget);
    }
    WiFi_WPSLoadSSID();
  }

  virtual void WiFi_Off()
  {
    isOn = false; isBusy = false; isAP = false; WiFi.mode(WIFI_OFF);
  }

  bool WiFi_IsSavedSSID()
  {
    return (wpsSSID[0] > 0);
  }

  virtual const char* WiFi_SavedSSID()
  {
    return wpsSSID;
  }

  virtual IPAddress WiFi_IP()
  {
    return isAP ? WiFi.softAPIP() : WiFi.localIP();
  }

  virtual void WiFi_loop()
  {
    switch (phase)
    {
    case WiFiPhase::Off:
    case WiFiPhase::Connected:
      break;

    case WiFiPhase::SD:
      if (WiFi.status() == WL_CONNECTED)
        phase = WiFiPhase::Connected;
      else
      if (phaseTimeout < millis())
      {
        phase = WiFiPhase::Off;
        D_println(S_WiFi_Msg(WiFi_Msg::Timeout));
        wifiMsg = S_WiFi_Msg(WiFi_Msg::Off);
        WiFi_Disconnect(false);
      }
      break;

    case WiFiPhase::Saved:
      if (WiFi.status() == WL_CONNECTED)
        phase = WiFiPhase::Connected;
      else
      if (phaseTimeout < millis())
      {
        phase = WiFiPhase::WPS;
        D_println(S_WiFi_Msg(WiFi_Msg::Timeout));
        WiFi_TryWPS();
      }
      break;

    case WiFiPhase::WPS:
      if (WiFi.status() == WL_CONNECTED)
        phase = WiFiPhase::Connected;
      else
      if (phaseTimeout < millis())
      {
        phase = WiFiPhase::Off;
        D_println(S_WiFi_Msg(WiFi_Msg::Timeout));
        wifiMsg = S_WiFi_Msg(WiFi_Msg::Off);
        WiFi_Disconnect(false);
      }
      break;

    case WiFiPhase::AP:
      if (WiFi.status() == WL_CONNECTED)
        phase = WiFiPhase::Connected;
      break;
    }
  }

  virtual void WiFi_WPSInitConfig(const char* deviceName)
  {
    wpsConfig.wps_type = ESP_WPS_MODE;
    strcpy(wpsConfig.factory_info.manufacturer, ESP_MANUFACTURER);
    strcpy(wpsConfig.factory_info.model_number, ESP_MODEL_NUMBER);
    strcpy(wpsConfig.factory_info.model_name, ESP_MODEL_NAME);
    strcpy(wpsConfig.factory_info.device_name, deviceName);
  }

  virtual void WiFi_WPSLoadSSID()
  {
    wifi_config_t wifiConfig;
    WiFi.mode(WIFI_STA);
    esp_wifi_get_config(WIFI_IF_STA, &wifiConfig);
    WiFi_Off();
    wpsSSID[0] = 0;
    strcpy(wpsSSID, (const char*)wifiConfig.sta.ssid);
  }

  //------------------------------------------------------------------------------
  // WiFi connect

  virtual void WiFi_ConnectWPS()
  {
    isOn = true;
    isBusy = true;
    WiFi_TrySaved();
  }

  virtual void WiFi_ConnectSD()
  {
    isOn = true;
    isBusy = true;
    WiFi_TrySD();
  }

  virtual void WiFi_ConnectAP()
  {
    isOn = true;
    isBusy = true;
    WiFi_TryAP();
  }

  //------------------------------------------------------------------------------
  // WiFi try connect

  virtual bool WiFi_TrySD()
  {
    bool result = false;
    wifiMsg = S_WiFi_Msg(WiFi_Msg::SSIDPassword);
    D_println(wifiMsg);
    File root = SD.open("/");
    File file;
    while ((file = root.openNextFile()))
    {
      if (!strcasecmp((const char*)file.name(), SSID_PASS_FileName))
      {
        char buffer[(128)];
        char ssid[(64)];
        char pass[(64)];
        D_println(file.name());
        file.readBytes(buffer, (128-1));
        file.close();
        SD.remove(SSID_PASS_FileName);

        if (2 == sscanf(buffer, "%63[^,],%63[^,]", ssid, pass))
        {
          WiFi.mode(WIFI_MODE_STA);
          wifiMsg = S_WiFi_Msg(WiFi_Msg::Connecting);
          D_println(wifiMsg);
          WiFi.begin(ssid, pass);

          result = true;
        }

        break;
      }
      file.close();
    }

    phaseTimeout = millis() + WiFi_TimeoutSD;
    phase = WiFiPhase::SD;

    return result;
  }

  void WiFi_TrySaved()
  {
    WiFi.mode(WIFI_MODE_STA);
    // re-connect; try for stored credentials first
    wifiMsg = S_WiFi_Msg(WiFi_Msg::Connecting);
    D_println(wifiMsg);
    WiFi.begin();

    phaseTimeout = millis() + WiFi_TimeoutSaved;
    phase = WiFiPhase::Saved;
  }

  void WiFi_TryWPS()
  {
    wifiMsg = S_WiFi_Msg(WiFi_Msg::PressWPS);
    D_println(wifiMsg);
    esp_wifi_wps_disable();
    esp_wifi_wps_enable(&wpsConfig);
    esp_wifi_wps_start(0);

    phaseTimeout = millis() + WiFi_TimeoutWPS;
    phase = WiFiPhase::WPS;
  }

  void WiFi_TryAP()
  {
    wifiMsg = S_WiFi_Msg(WiFi_Msg::APMode);
    D_println(wifiMsg);
    WiFi.mode(WIFI_AP_STA);
    wpsSSID[0] = 0;
    isAP = false;
    if (WiFi.softAP(hostName, "", WiFi_Channel, false, WiFi_MaxConnections))
    {
      isAP = true;
      phase = WiFiPhase::Connected;
    }
  }

  virtual void WiFi_OnEvent(WiFiEvent_t event, WiFiEventInfo_t info)
  {
    wifiEvent = event;

    switch ((int)wifiEvent)
    {
    case WIFI_EVENT_STA_START:// 2
      {
        wifiMsg = S_WiFi_Msg(WiFi_Msg::STAMode);
        D_println(wifiMsg);
      }
      break;

    case WIFI_EVENT_STA_STOP:// 3
      {
        wifiMsg = S_WiFi_Msg(WiFi_Msg::Off);
        D_println("STA stopped");
      }
      break;

    case IP_EVENT_STA_GOT_IP:// 7
      {
        D_print("SSID: ");
        D_println(WiFi.SSID());
        D_print("  IP: ");
        D_println(WiFi_IP());
        isBusy = false;
      }
      break;

    case WIFI_EVENT_STA_DISCONNECTED:// 5
      {
        wifiMsg = S_WiFi_Msg(WiFi_Msg::Reconnect);
        D_println(wifiMsg);
        WiFi.reconnect();
      }
      break;

    case WIFI_EVENT_STA_WPS_ER_SUCCESS:// 9
      {
        D_println("WPS ok, stop WPS and connect to: " + WiFi.SSID());
        wifiMsg = S_WiFi_Msg(WiFi_Msg::WPSOk);
        D_println(wifiMsg);
        esp_wifi_wps_disable();
        delay(10);
        WiFi.begin();
      }
      break;

    case WIFI_EVENT_STA_WPS_ER_FAILED:// 10
      {
        wifiMsg = S_WiFi_Msg(WiFi_Msg::WPSFailRetry);
        D_println(wifiMsg);
        esp_wifi_wps_disable();
        esp_wifi_wps_enable(&wpsConfig);
        esp_wifi_wps_start(0);
      }
      break;

    case WIFI_EVENT_STA_WPS_ER_TIMEOUT:// 11
      {
        wifiMsg = S_WiFi_Msg(WiFi_Msg::WPSTimeoutRetry);
        D_println(wifiMsg);
        esp_wifi_wps_disable();
        esp_wifi_wps_enable(&wpsConfig);
        esp_wifi_wps_start(0);
      }
      break;

      // case WIFI_EVENT_STA_WPS_ER_PIN:// 12

    case WIFI_EVENT_AP_START:
      {
        wifiMsg = S_WiFi_Msg(WiFi_Msg::APStart);
        D_println(wifiMsg);
        D_print("SSID: ");
        D_println(hostName);
        D_print("  IP: ");
        D_println(WiFi_IP());
        isBusy = false;
      }
      break;

    case WIFI_EVENT_AP_STOP:
      {
        wifiMsg = S_WiFi_Msg(WiFi_Msg::APStop);
        D_println(wifiMsg);
      }
      break;

    // case WIFI_EVENT_AP_STACONNECTED:
    //   wifiMsg = S_WiFi_Msg(WiFi_Msg::APSTAConnected);
    //   D_println(wifiMsg);
    //   break;

    // case WIFI_EVENT_AP_STADISCONNECTED:
    //   wifiMsg = S_WiFi_Msg(WiFi_Msg::APSTADisconnected);
    //   D_println(wifiMsg);
    //   break;

    // case WIFI_EVENT_AP_STAIPASSIGNED:
    //   break;

    // case WIFI_EVENT_AP_PROBEREQRECVED:
    //   break;

      default:
        break;
    }
  }
};

}// namespace KontrolRack

#endif// KontrolRack_ESPWiFi_h