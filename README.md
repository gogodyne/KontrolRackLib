# KontrolRackLib
The "KontrolRack" is a proposed standard for modular USB control panels.

- [https://gogodyne.co/KontrolRack/](https://gogodyne.co/KontrolRack/)

This Arduino-compatible library can be used to program microcontrollers for use with KontrolRack Modules. The library supports multiple Banks of controls, such as an array of meters with OLED displays for labels. The ESP32 family of microcontroller is preferred for its speed, memory, and wireless capabilities, but any equivalent should work.

With its own dedicated microcontroller, each KontrolRack Module can be a fully standalone USB device by taking advantage of a development board's USB-to-Serial interface. Devices are powered through the USB interface, requiring no additional power supply.

The library relies on I2C devices attached to an I2C bus in order to reduce wiring connections. Using an I2C switch (MUX) allows for Banks of identical devices using their default I2C addresses. An OLED display can be refreshed at about 30 FPS, but 4 Banks will still run at almost 8 FPS each.

## Dependencies
The KontrolRack Library makes use of several other Arduino built-in libraries, depending on what hardware is involved.
- Preferences
- Wire
- SparkFun I2C Mux Arduino Library
- Adafruit SSD1306
- Adafruit GFX Library
- Adafruit BusIO
- Hash
- SPI
- SD
- FS
- SPIFFS
- WiFi
- Networking
- ESP Async WebServer
- Async TCP
- QRCodeGFX
- KerbalSimpit

## Examples
The KontrolRack Library is intended as a starting point for specific uses. It does not by itself decide what to display or how to communicate.

Included are examples of firmware for control panel Modules for the game "Kerbal Space Program" (KSP and KSP2).

- [https://docs.arduino.cc/libraries/kerbalsimpit/](https://docs.arduino.cc/libraries/kerbalsimpit/)

Features in the examples include:
- Example Module firmware: 4-bank LED meters with 128x32 OLED labels, with a rotary dial.
- Example Module firmware: 4-bank numeric LEDs with 128x32 OLED labels, with a rotary dial.
- 16 presets, selectable and editable on the Module using the rotary dial, stored to non-volatile memory.
- Hold the rotary button to access the menu.
- WiFi Web configuration page, for alternative editing.
- QR code display for the microcontroller's SSID to join its WiFi Access Point, i.e. "KontrolRackXXXX".
- QR code display for the microcontroller's IP address to reach the configuration page, i.e. "192.168.4.1".
- Screensaver: periodic reverse-video to reduce screen burn-in.
