#include "Pong.h"
#include "Kernel.h"
#include "MyPrint.h"

Pong::Pong(Kernel* kernel) : App(kernel) {
  reset_ball();  
  this->kernel = kernel;


  kernel->display.setTextColor(TFT_WHITE, TFT_BLACK, true);
  kernel->display.setTextDatum(MC_DATUM);
  kernel->loadSmallFont();
}

String Pong::get_name() {
  return String("Pong");
}

short randPlusOrMinus() {
  if(random(2) == 0) {
    return 1;
  } else {
    return -1;
  }
}

void Pong::run_code(double x, double y, bool special) {
  kernel->drawViewBox();

  RectDouble user_last = user_move(x, y);
  RectDouble comp_last = comp_move();
  RectDouble ball_last = ball_move();

  kernel->display.fillRect(comp_last.x, comp_last.y, comp_last.width, comp_last.height, BLACK);
  kernel->display.fillRect(comp_pad .x, comp_pad .y, comp_pad .width, comp_pad .height, WHITE);

  kernel->display.fillRect(user_last.x, user_last.y, user_last.width, user_last.height, BLACK);
  kernel->display.fillRect(user_pad .x, user_pad .y, user_pad .width, user_pad .height, WHITE);

  kernel->display.fillCircle(ball_last.x + BALL_DIAMETER/2, ball_last.y + BALL_DIAMETER/2, BALL_DIAMETER/2, BLACK);
  kernel->display.fillCircle(ball     .x + BALL_DIAMETER/2, ball     .y + BALL_DIAMETER/2, BALL_DIAMETER/2, WHITE);

  kernel->display.fillRect((screen_width - 2) / 2, top_vb, 2, viewbox_wh, WHITE); //Center line
  
  this->printScores();
}

void Pong::printScores() {
  kernel->display.setTextColor(TFT_RED, TFT_BLACK, true);
  kernel->display.drawString(format0(this->comp_score), center_x - viewbox_wh / 4, top_vb + SCORE_Y);

  // kernel->display.setCursor(USER_SCORE_X, SCORE_Y);
  kernel->display.setTextColor(TFT_BLUE, TFT_BLACK, true);
  kernel->display.drawString(format0(this->user_score), center_x + viewbox_wh / 4, top_vb + SCORE_Y);
}

RectDouble Pong::user_move(double x, double y) {
  RectDouble last = user_pad;
  
  double fps_ratio = 80.00 / kernel->get_fps(); 
  // user_pad.y = min(, max(, ));

  user_pad.y = constrain(user_pad.y + fps_ratio * y, double(top_vb), double(bottom_vb - user_pad.height));

  return last;
}

RectDouble Pong::comp_move() {
  RectDouble last = comp_pad;

  double dif = (ball.y - ball.height / 2) - (comp_pad.y - comp_pad.height / 2);
  double mov = comp_speed * abs(dif) / dif;
  double fps_ratio = 80.00 / kernel->get_fps(); 

  // comp_pad.y = min(double(kernel->display.height() - comp_pad.height), max(0.00, comp_pad.y + fps_ratio * mov));

  comp_pad.y = constrain(comp_pad.y + fps_ratio * mov, double(top_vb), double(bottom_vb - comp_pad.height));


  return last;
}

RectDouble Pong::ball_move() {
  RectDouble last = ball;
  
  double fps_ratio = 80.00 / kernel->get_fps(); 
  
  //Move forward
  ball.x += ball_speed * fps_ratio * cos(ball_angle);
  ball.y += ball_speed * fps_ratio * sin(ball_angle); 
  // ball.x  = min(max(ball.x, 0.00), double(SCREEN_WIDTH  - BALL_DIAMETER));
  ball.x = constrain(ball.x, left_vb, right_vb);
  // ball.y  = min(max(ball.y, 0.00), double(SCREEN_HEIGHT - BALL_DIAMETER));
  ball.y = constrain(ball.y, top_vb, bottom_vb - BALL_DIAMETER);

  if(ball.y == top_vb or ball.y == bottom_vb - BALL_DIAMETER) {
    ball_angle *= -1;
  }
  
  //Check paddles
  if(not check_collisions()) 
  {
    if(ball.x <= left_vb) {
      user_score++;
      reset_ball();
    } else if (ball.x >= right_vb - BALL_DIAMETER) {
      comp_score++;
      reset_ball();
    }
  }
  
  return last;
}

void Pong::reset_ball() {
  ball.x = (screen_width  - BALL_DIAMETER) / 2;
  ball.y = (screen_height - BALL_DIAMETER) / 2;

  ball_angle = randPlusOrMinus() * (pi + randPlusOrMinus() / 100.00 * random(20, 100)); //Start roughly straight
}

bool Pong::check_collisions() {
  double dif; 
  if(ball.intersect(user_pad)) {
    dif = 0;//( (ball.y - ball.height / 2) - (user_pad.y - PAD_HEIGHT / 2) )  /  (BALL_DIAMETER + PAD_HEIGHT);
    ball_angle *= -1;
    ball_angle += pi - dif;
    ball.x = user_pad.x - ball.width - 1; //Moves it out of the pad so it doesn't do weird sht
    return true;
  } else if (ball.intersect(comp_pad)) {
    dif = 0;// ( (ball.y - BALL_DIAMETER / 2) - (comp_pad.y - PAD_HEIGHT / 2) )  /  (BALL_DIAMETER + PAD_HEIGHT);
    ball_angle *= -1;
    ball_angle += pi - dif;
    ball.x = comp_pad.x + comp_pad.width + 1;
    return true;
  } else {
    return false;
  }
}
