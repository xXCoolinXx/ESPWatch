#include "App.h"

class Kernel;

enum SnakeDir {
  NONE, 
  UP, DOWN, LEFT, RIGHT,
}

SnakeDir getdirFromJoystick(double x, double y);

class Snake : public App {
  public:
  Snake(Kernel* kernel);
  void run_code(double x, double y, bool special, Kernel* kernel);
  String get_name();
};
