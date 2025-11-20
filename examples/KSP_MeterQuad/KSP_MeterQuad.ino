#include "KSP_Settings.h"
#include "KSP_MeterQuad.h"

KSP_MeterQuad myKRModule(Wire);

void setup() {
  // put your setup code here, to run once:

  // Start the communication ports
  Serial.begin(115200);
  Wire.begin();

  // Let devices power up...
  delay(1000);

  // Start the KR Module
  myKRModule.begin(messageHandler);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Run the KR Module
  myKRModule.loop();
}

// Messages from the Simpit
void messageHandler(byte messageType, byte msg[], byte msgSize) {
  myKRModule.messageHandler(messageType, msg, msgSize);
}
