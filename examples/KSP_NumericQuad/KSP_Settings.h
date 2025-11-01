// (c) 2025 gogodyne
#ifndef KSP_Settings_h
#define KSP_Settings_h

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

// Numeric
#define NUM8_FPS        12
#define NUM8_INTENSITY  .1f
#if defined(ARDUINO_XIAO_ESP32C3)
#define NUM8_DIN  10
#define NUM8_CS   9
#define NUM8_CLK  8
#endif// defined(board)
// default ESP32
#if !defined(NUM8_DIN)
#if defined(ESP32)
#define NUM8_DIN  15
#define NUM8_CS   2
#define NUM8_CLK  4
#endif// defined(board)
#endif// defined(NUM8_DIN)

#define MODULE_FPS 12

#endif// KSP_Settings_h
