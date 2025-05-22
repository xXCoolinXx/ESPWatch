#include "App.h"

class Kernel;

enum SnakeDir {
  SNAKE_NONE, 
  SNAKE_UP, SNAKE_DOWN, SNAKE_LEFT, SNAKE_RIGHT,
};

SnakeDir getdirFromJoystick(double x, double y);

class Snake : public App {
  Kernel* kernel;
  public:
  Snake(Kernel* kernel);
  void run_code(double x, double y, bool special);
  String get_name();
};
