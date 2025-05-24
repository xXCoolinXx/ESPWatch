#include "Start.h"
#include "Kernel.h"
#include "ClockDaemon.h"
#include "MyPrint.h"
#include "Clock.h"
#include "Images.h"
#include "Pong.h"
#include "Snake.h"
#include "News.h"
#include <cmath>

//smaller windows like cursor

Start::Start(Kernel* kernel) : App(kernel) {
  this->kernel = kernel;
  // Cursor = RectDouble{kernel->display.width() / 2, kernel->display.height() / 2, 4, 4};
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
  // make_sprite(cursor, &(this->kernel->display), ursor.width, Cursor.height, cursor_bmp);
}

void Start::move_cursor(double x, double y) {
  if(!this->showTime) {
    kernel->display.drawRoundRect(
      app_rects[this->Cursor.y][this->Cursor.x].x - OUTLINE_APP_DIF - 1, 
      app_rects[this->Cursor.y][this->Cursor.x].y - OUTLINE_APP_DIF - 1, 
      app_rects[this->Cursor.y][this->Cursor.x].width + 2, 
      app_rects[this->Cursor.y][this->Cursor.x].height + 2,
      1, 
      TFT_BLACK
    );
  }

  if(x != 0 && (this->last_x == 0.0 || this->last_x == -10.0)) {
    Cursor.x += (x > 0 ? 1 : -1);
    Cursor.x = (Cursor.x % COLS + COLS) % COLS;
  }
  if(y != 0 && (this->last_y == 0.0 || this->last_y == -10.0)) {
    Cursor.y += (y > 0 ? 1 : -1);
    Cursor.y = (Cursor.y % ROWS + ROWS) % ROWS;
  }

  if(!this->showTime) {
    kernel->display.drawRoundRect(
      app_rects[this->Cursor.y][this->Cursor.x].x - OUTLINE_APP_DIF - 1, 
      app_rects[this->Cursor.y][this->Cursor.x].y - OUTLINE_APP_DIF - 1, 
      app_rects[this->Cursor.y][this->Cursor.x].width + 2, 
      app_rects[this->Cursor.y][this->Cursor.x].height + 2,
      1, 
      TFT_WHITE
    );
  }
}

void Start::display_cursor() {
  // this->cursor->pushSprite(lastCursor.x, lastCursor.y);
  // kernel->display.drawBitmap(lastCursor.x, lastCursor.y, cursor, lastCursor.width, lastCursor.height, (lastCursor.y >= 8) ? BLACK : WHITE);
  // kernel->display.drawBitmap(    Cursor.x,     Cursor.y, cursor,     Cursor.width,     Cursor.height, (    Cursor.y >= 8) ? WHITE : BLACK);
}

void Start::run_code(double x, double y, bool special) {
  // Serial.println(x,y);
  // move_cursor(x, y);
  // checkPress(special);

  // display_cursor();

  //Swap from showing Time to Apps or vice versa
  if(y != 0 && (this->last_y == 0.0 || this->last_y == -10.0)) {
    if(this->showTime || (y < 0 && Cursor.y == 0) || (y > 0 && Cursor.y == ROWS - 1)) {
      this->showTime = ! this->showTime;
      kernel->clearViewBox();
    }
  }

  if(this->showTime) {
    this->display_time();
  } else {
    this->displayApps();
    this->checkPress(special);
    this->move_cursor(x, y);
  }

  // Serial.println((special ? "true" : "false"));

  last_x = x;
  last_y = y;
}

String Start::get_name() {
  return String("Start");
}

void Start::checkPress(bool special) {
  if (special) {
    if(this->Cursor.x == 0 && this->Cursor.y == 0) {
      Serial.println("selecting pong");
      kernel->set_app(new Pong(kernel)); 
    } else if (this->Cursor.x == 1 && this->Cursor.y == 0) {
      Serial.println("selecting snake");
      kernel->set_app(new Snake(kernel));
    } else if (this->Cursor.x == 2 && this->Cursor.y == 0) {
      kernel->set_app(new News(kernel));
    }
  }
}

void Start::display_time() {
  kernel->display.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tmElements_t tm = kernel->_clock->get_time();
  kernel->display.setTextDatum(MC_DATUM);
  kernel->loadBigFont();
  print_time(tm, &(kernel->display), screen_width/2, screen_height/2 - 10);

  kernel->loadSmallFont();
  kernel->display.drawString(format0(tm.Month) + "/" + format0(tm.Day) + "/" + format0(tm.Year - 30), screen_width/2, screen_height/2 + 20); //tm.Year starts is 0@1970, so +1970 then -2000 = -30
}

void Start::displayApps() {
  //Pong
  this->pong->pushSprite(app_rects[0][0].x, app_rects[0][0].y);

  //Snake
  this->snake->pushSprite(app_rects[0][1].x, app_rects[0][1].y);

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
