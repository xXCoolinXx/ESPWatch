#include "Snake.h"
#include "Kernel.h"

SnakeDir getdirFromJoystick(double x, double y) {
  if(x > 0
}

Snake::Snake(Kernel* kernel) : App(kernel) {}

void Snake::run_code(double x, double y, bool special, Kernel* kernel) {
  kernel->display.print("SNAKE");  
}

String Snake::get_name() {
  return String("newApp");
}
