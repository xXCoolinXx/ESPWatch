#include "Snake.h"
#include "Kernel.h"

SnakeDir getdirFromJoystick(double x, double y) {
  if(x > 0) { return SNAKE_NONE; }
}

Snake::Snake(Kernel* kernel) : App(kernel) {
  this->kernel = kernel;
}

void Snake::run_code(double x, double y, bool special) {
  this->kernel->display.print("SNAKE");  
}

String Snake::get_name() {
  return String("newApp");
}
