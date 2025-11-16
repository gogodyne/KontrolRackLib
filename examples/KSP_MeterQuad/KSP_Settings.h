// (c) 2025 gogodyne
#ifndef KSP_Settings_h
#define KSP_Settings_h

#define APP_TITLE "KSP Meter Quad"

// Encoder
#define ROTENC_PositionCount 20
#if !defined(ROTENC_A)
#if defined(ARDUINO_XIAO_ESP32C3)
#define ROTENC_A 3
#define ROTENC_B 4
#define ROTENC_S 5
#endif// defined(board)
#if defined(ARDUINO_LOLIN_S2_MINI)
#define ROTENC_A 17
#define ROTENC_B 21
#define ROTENC_S 15
#endif// defined(board)
#endif// defined(ROTENC_A)
// default ESP32
#if !defined(ROTENC_A)
#if defined(ESP32)
#define ROTENC_A 16
#define ROTENC_B 17
#define ROTENC_S 5
#endif// defined(board)
#endif// defined(ROTENC_A)

// I2C
#define SWITCH_ADDRESS_MASTER 0x77
#define SWITCH_ADDRESS        (QWIIC_MUX_DEFAULT_ADDRESS)
#define SWITCH_ADDRESS_METER  (QWIIC_MUX_DEFAULT_ADDRESS + 1)// I2C switch and Adafruit bargraph have the same base address

// OLED
#define OLED12864_ADDRESS 0x3C
#define OLED12864_FPS     12

// LED24
#define LED24_FPS         8
#define LED24_Brightness  2//[0-15]
#define LED24_ADDRESS     (AdafruitBargraph_Address + 0)

#define MODULE_FPS        12

#define WIFI_AUTOSTART

#endif// KSP_Settings_h
