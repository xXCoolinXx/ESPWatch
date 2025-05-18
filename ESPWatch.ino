#include "Joystick.h"
#include "Color.h"
#include "Ports.h"
#include "Kernel.h"
#include "Start.h"

Kernel kernel;

void setup() {
  kernel.setupf();
}

void loop() {
  kernel.loopf();
}
