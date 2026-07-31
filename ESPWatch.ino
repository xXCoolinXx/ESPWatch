// #include "Joystick.h"
#include "src/utils/Color.h"
#include "src/config/Ports.h"
#include "src/kernel/Kernel.h"
#include "src/apps/Start.h"

Kernel kernel;

void setup() {
  kernel.setupf();
}

void loop() {
  kernel.loopf();
}
