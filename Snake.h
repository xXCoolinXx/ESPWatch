#include "App.h"
#include "Shapes.h"
#include "GFXConstants.h"

class Kernel;

enum SnakeDir {
  SNAKE_NONE, 
  SNAKE_UP, SNAKE_DOWN, SNAKE_LEFT, SNAKE_RIGHT,
};

const unsigned short snake_dim = 5; // Given current viewbox_wh, this gives a 25x25 grid
const double spm = 1.5; // Seconds per move (0.5 -> move twice in one second)
const int border_space = 1; // Spacing between apple and border when generating 

SnakeDir getdirFromJoystick(double x, double y);

// Linkeduhlist
struct SnakePart {
  RectInt part = RectInt{center_x - center_x%snake_dim - snake_dim, center_y - center_y%snake_dim - snake_dim, snake_dim, snake_dim};
  SnakeDir dir = SNAKE_NONE;
  SnakePart* next = nullptr;
};

class Snake : public App {
  Kernel* kernel;

  SnakePart* head = nullptr;

  RectInt apple = RectInt{0, 0, snake_dim, snake_dim};

  bool last_x = false; 
  bool last_y = false;

  double time_since_last_move = 0.0;
  public:
  Snake(Kernel* kernel);
  ~Snake();
  void run_code(double x, double y, bool special);
  void delete_snake();

  SnakeDir getdirFromJoystick(double x, double y);

  void move_apple();

  String get_name();
};
