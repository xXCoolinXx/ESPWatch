#include "App.h"
#include "Shapes.h"

#define PAD_WIDTH      2
#define PAD_HEIGHT    12
#define SCREEN_WIDTH  96
#define SCREEN_HEIGHT 64
#define BALL_SPEED     3  
#define BALL_DIAMETER  5
#define COMP_SCORE_X  18
#define USER_SCORE_X  68
#define SCORE_Y       5
const double pi = 4 * atan(1.0);

class Kernel;

class Pong : public App {
  RectDouble comp_pad{0, (SCREEN_HEIGHT - PAD_HEIGHT) / 2, PAD_WIDTH, PAD_HEIGHT};
  RectDouble user_pad{SCREEN_WIDTH - PAD_WIDTH, (SCREEN_HEIGHT - PAD_HEIGHT) / 2, PAD_WIDTH, PAD_HEIGHT};
  RectDouble ball{0, 0, BALL_DIAMETER, BALL_DIAMETER};
  
  const double ball_speed = 1.10;
  const double comp_speed = 0.75;
  double ball_angle;

  int user_score = 0;
  int comp_score = 0;

  public:
    Pong(Kernel* kernel);
    void run_code(double x, double y, bool special, Kernel* kernel);
    String get_name();
    RectDouble user_move(double x, double y, Kernel* kernel);
    RectDouble comp_move(Kernel* kernel);
    RectDouble ball_move(Kernel* kernel);
    void reset_ball();
    bool check_collisions();
    
};
