#include <Wire.h>
#include <VPortServer.h>

#define ADDR 80

VPortServer i2cSrv(Wire);

void setup() {
  i2cSrv.begin(ADDR);
}

void loop() {}
