#include "Start.h"
#include "Kernel.h"
#include "ClockDaemon.h"
#include "MyPrint.h"
// #include "Clock.h"
#include "Images.h"
#include "Pong.h"
#include "Snake.h"
#include "News.h"
#include <cmath>

//smaller windows like cursor

Start::Start(Kernel* kernel) : App(kernel) {
  this->kernel = kernel;
  this->_setup_sprites();

  int width = OUTLINE_DIM*COLS + APP_SPACING_X*(COLS - 1);
  int height = OUTLINE_DIM*ROWS + APP_SPACING_Y*(ROWS - 1);

  int start_x = (screen_width - width) / 2;
  int start_y = (screen_height - height) / 2; 

  for(int i = 0; i < ROWS; i++) {
    for(int j = 0; j < COLS; j++) {
      app_rects[i][j] = RectInt{
        start_x + OUTLINE_APP_DIF + j * (OUTLINE_DIM + APP_SPACING_X), 
        start_y + OUTLINE_APP_DIF + i * (OUTLINE_DIM + APP_SPACING_Y),
        OUTLINE_DIM, 
        OUTLINE_DIM
      };
    }
  }
}

Start::~Start() {
  delete pong;
  delete snake;
  // delete cursor;
}

void Start::_setup_sprites() {
  make_sprite(pong, &(this->kernel->display), APP_DIM, APP_DIM, pong18);
  make_sprite(snake, &(this->kernel->display), APP_DIM, APP_DIM, snake18);
  make_sprite(news, &(this->kernel->display), APP_DIM, APP_DIM, news18);
}

void Start::run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt) {
  // Toggle app view vs digital clock view on tap
  if(gesture && pt && gesture.value() == TouchHandler::Gesture::TAP) {
    bool in_viewbox = viewbox_rect.contains(pt.value());
    
    if(this->showTime || !in_viewbox) { 
      this->showTime = ! this->showTime;
      kernel->clearViewBox();
    } else if(in_viewbox) {
      this->checkPress(pt.value());
    }
  }

  if(this->showTime) {
    this->display_time();
  } else {
    this->displayApps();
  }
}

String Start::get_name() {
  return String("Start");
}

void Start::checkPress(const PointInt& pt) {
  if(app_rects[0][0].contains(pt)) {
    Serial.println("selecting pong");
    kernel->set_app(new Pong(kernel)); 
  } else if(app_rects[0][1].contains(pt)) {
    Serial.println("selecting snake");
    kernel->set_app(new Snake(kernel)); 
  } else if (app_rects[0][2].contains(pt)) {
    Serial.println("selecting news");
    kernel->set_app(new News(kernel));
  }
}

void Start::display_time() {
  kernel->display.setTextColor(TFT_WHITE, TFT_BLACK, true);
  struct tm current_time = kernel->_clock->get_time();
  kernel->display.setTextDatum(MC_DATUM);
  kernel->loadBigFont();
  print_time(current_time, &(kernel->display), screen_width/2, screen_height/2 - 10);

  kernel->loadSmallFont();
  kernel->display.drawString(
      format0(current_time.tm_mon + 1) + "/" + 
      format0(current_time.tm_mday) + "/" + 
      format0(current_time.tm_year%100), 
      screen_width/2, 
      screen_height/2 + 20
  ); //tm.Year starts is 0@1970, so +1970 then -2000 = -30
  
  kernel->display.drawString(
    std::to_string(kernel->getBatteryLevel()).c_str(),
    screen_width/2,
    screen_height/2 + 40
  );

  kernel->display.drawString(
    std::to_string(kernel->getPodometerCount()).c_str(),
    screen_width/2,
    screen_height/2 + 60
  );
}

void Start::displayApps() {
  //Pong
  this->pong->pushSprite(app_rects[0][0].x, app_rects[0][0].y);

  //Snake
  this->snake->pushSprite(app_rects[0][1].x, app_rects[0][1].y);

  this->news->pushSprite(app_rects[0][2].x, app_rects[0][2].y);

  for(int i = 0; i < ROWS; i++) {
    for(int j = 0; j < COLS; j++) {
      kernel->display.drawRoundRect(
        app_rects[i][j].x - OUTLINE_APP_DIF, 
        app_rects[i][j].y - OUTLINE_APP_DIF, 
        app_rects[i][j].width, 
        app_rects[i][j].height,
        1, 
        TFT_WHITE);
    }
  }
}
