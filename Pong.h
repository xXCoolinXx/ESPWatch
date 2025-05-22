#include "App.h"
#include "Shapes.h"
#include "GFXConstants.h"

#define PAD_WIDTH      2
#define PAD_HEIGHT    15
#define PAD_OFFSET 3
#define BALL_SPEED     3  
#define BALL_DIAMETER  5
#define COMP_SCORE_X  18
#define USER_SCORE_X  68
#define SCORE_Y       15
const double pi = 4 * atan(1.0);

class Kernel;

class Pong : public App {
  RectDouble comp_pad{(screen_width - viewbox_wh)/2 + PAD_OFFSET, (screen_height - PAD_HEIGHT) / 2, PAD_WIDTH, PAD_HEIGHT};
  RectDouble user_pad{(screen_width + viewbox_wh)/2 - PAD_WIDTH - PAD_OFFSET, (screen_height - PAD_HEIGHT) / 2, PAD_WIDTH, PAD_HEIGHT};
  RectDouble ball{screen_width/2, screen_height/2, BALL_DIAMETER, BALL_DIAMETER};
  
  const double ball_speed = 1.10;
  const double comp_speed = 0.75;
  double ball_angle;

  int user_score = 0;
  int comp_score = 0;

  Kernel* kernel;
  public:
    Pong(Kernel* kernel);
    void run_code(double x, double y, bool special);
    String get_name();
    RectDouble user_move(double x, double y);
    RectDouble comp_move();
    RectDouble ball_move();
    void reset_ball();
    bool check_collisions();
    
    void printScores();
};
