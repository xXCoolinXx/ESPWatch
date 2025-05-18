#include "Start.h"
#include "Kernel.h"
#include "ClockDaemon.h"
#include "Print.h"
#include "Clock.h"
#include "Images.h"
#include "Pong.h"
#include "Snake.h"
#include <cmath>

//smaller windows like cursor
const unsigned char PROGMEM cursor[]
{
  0xe0,
  0xc0,
  0xa0,
  0x10
};

Start::Start(Kernel* kernel) : App(kernel) {
  Cursor = RectDouble{kernel->display.width() / 2, kernel->display.height() / 2, 4, 4};
}

void Start::move_cursor(double x, double y, Kernel* kernel) {
  lastCursor = Cursor;
  
  double fps_ratio = 70.00 / kernel->get_fps(); //Make sure cursor moves at the same speed always
  Cursor.x = min(double(kernel->display.width() - Cursor.width), max(0.00, Cursor.x + fps_ratio * x) );
  Cursor.y = min(double(kernel->display.height() - Cursor.height), max(0.00, Cursor.y + fps_ratio * y) );
}

void Start::display_cursor(Kernel* kernel) {
  kernel->display.drawBitmap(lastCursor.x, lastCursor.y, cursor, lastCursor.width, lastCursor.height, (lastCursor.y >= 8) ? BLACK : WHITE);
  kernel->display.drawBitmap(    Cursor.x,     Cursor.y, cursor,     Cursor.width,     Cursor.height, (    Cursor.y >= 8) ? WHITE : BLACK);
}

void Start::run_code(double x, double y, bool special, Kernel* kernel) {
   move_cursor(x, y, kernel);
   checkPress(special, kernel);
  
   display_cursor(kernel); 
   display_time(kernel);
   displayApps(kernel);
}

String Start::get_name() {
  return String("Start");
}

void Start::checkPress(bool special, Kernel* kernel) {
  if (special) {
    if (Cursor.y < 8) {
      kernel->set_app(new Clock(kernel));
    } else {
      if(app_rects[0].contains(Cursor.toPointInt())) {
        kernel->set_app(new Pong(kernel)); 
      } else if(app_rects[1].contains(Cursor.toPointInt())) {
        kernel->set_app(new Snake(kernel));
      }
    }
  }
}

void Start::display_time(Kernel* kernel) {
  
  if (Cursor.y < 8) {
    kernel->display.setTextColor(BLACK);
    if (lastCursor.y >= 8) {
      kernel->display.fillRect(0, 0, kernel->display.width(), 9, WHITE);
    }
  } else if (lastCursor.y < 8) {
    kernel->display.fillRect(0, 0, kernel->display.width(), 9, BLACK);
  }
  
  kernel->display.fillRect(0, 9, kernel->display.width(), 2, WHITE);
  
  tmElements_t tm = kernel->_clock->get_time();
  time_t tmt = makeTime(tm);

  Adafruit_SSD1331 * display = &(kernel->display);
  
  display->setCursor(0, 0);
  print2(hourFormat12(tmt), display);
  display->print(":");
  print2(tm.Minute, display);
  if(isAM(tmt)) {
    display->print("AM ");
  } else {
    display->print("PM ");
  }

  print2(tm.Month, display);
  display->print("/");
  print2(tm.Day, display);
  display->print("/");
  print2(1970 + tm.Year - 2000, display);

  if (kernel->_clock->minuteChanged()) display->fillRect(0, 0, display->width(), 9, (Cursor.y < 8) ? WHITE : BLACK);
}

void Start::displayApps(Kernel* kernel) {
  for(int i = 0; i < ROWS*COLS; i++) {
    kernel->display.drawRoundRect(app_rects[i].x - 1, app_rects[i].y - 1, OUTLINE_DIM, OUTLINE_DIM, 1, WHITE);
  }
  
  //Pong
  kernel->display.drawRGBBitmap(app_rects[0].x, app_rects[0].y, pong18, APP_DIM, APP_DIM); 

  //Snake
  kernel->display.drawRGBBitmap(app_rects[1].x, app_rects[1].y, snake18, APP_DIM, APP_DIM);
}
