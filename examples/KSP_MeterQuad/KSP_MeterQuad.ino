#include "KSP_Settings.h"
#include "KSP_MeterQuad.h"

KSP_MeterQuad rackModule(Wire);

void setup() {
  // put your setup code here, to run once:

  // Start the communication ports
  Serial.begin(115200);
  Wire.begin();

  // Let devices power up...
  delay(1000);

  // Start the Module
  rackModule.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  // Run the Module
  rackModule.loop();
}
