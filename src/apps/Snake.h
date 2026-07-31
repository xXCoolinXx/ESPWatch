#ifndef SNAKE_H
#define SNAKE_H
#include "src/apps/App.h"
#include "src/utils/Shapes.h"
#include "src/config/GFXConstants.h"
#include <deque>

class Kernel;

enum SnakeDir {
  SNAKE_NONE, 
  SNAKE_UP, SNAKE_DOWN, SNAKE_LEFT, SNAKE_RIGHT,
};

const unsigned short snake_dim = 5; // Given current viewbox_wh, this gives a 25x25 grid
const double spm = 1.5; // Seconds per move (0.5 -> move twice in one second)
const int border_space = 1; // Spacing between apple and border when generating 

// Snake body segment. Stored in a std::deque (front = head, back = tail),
// so no manual memory management is needed.
struct SnakePart {
  RectInt part = RectInt{center_x - center_x%snake_dim - snake_dim, center_y - center_y%snake_dim - snake_dim, snake_dim, snake_dim};
  SnakeDir dir = SNAKE_NONE;

  SnakePart(const RectInt& p = RectInt{center_x - center_x%snake_dim - snake_dim, center_y - center_y%snake_dim - snake_dim, snake_dim, snake_dim}, SnakeDir d = SNAKE_NONE)
  : part(p), dir(d) {}
};

class Snake : public App {

  std::deque<SnakePart> body;

  RectInt apple = RectInt{0, 0, snake_dim, snake_dim};

  double time_since_last_move = 0.0;

  TouchHandler::oGesture cached_gesture;
  public:
  Snake(Kernel& kernel);
  void run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt);
  void delete_snake();

  SnakeDir getDirFromTouch(const TouchHandler::oGesture& gesture, const oPointInt& pt);

  void move_apple();

  String get_name();
};
#endif // SNAKE_H
