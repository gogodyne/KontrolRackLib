#include "KSP_Settings.h"
#include "KSP_Autopilot.h"

using namespace KontrolRack;

KSP_Autopilot myKRModule;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  myKRModule.begin(messageHandler, BTN8_PININ, BTN8_PINOUT, BTN9_PININ, BTN9_PINOUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  myKRModule.loop();
}

// Messages from the Simpit
void messageHandler(byte messageType, byte msg[], byte msgSize) {
  myKRModule.messageHandler(messageType, msg, msgSize);
}
