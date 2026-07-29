#pragma once
#include "App.h"
#include "Shapes.h"
#include <TFT_eSPI.h>

const int APP_DIM = 18;
const int OUTLINE_DIM = APP_DIM + 2;
const int OUTLINE_APP_DIF = (OUTLINE_DIM - APP_DIM)/2;
const int APP_SPACING_X = 10; //96 - 3 * OUTLINE_DIM / 4
const int APP_SPACING_Y = 10;
const int ROWS = 3;
const int COLS = 3;

class Start : public App {
  private: 
    // RectDouble Cursor;
    // RectDouble lastCursor;
    Kernel* kernel;
    double last_x = -10.0;
    double last_y = -10.0;
    bool showTime = true;

    RectInt app_rects[ROWS][COLS];
    void _setup_sprites();

    // Apps
    TFT_eSprite* pong = nullptr;
    TFT_eSprite* snake = nullptr;

    // TFT_eSprite* cursor = nullptr;
  public:
  Start(Kernel* kernel);
  ~Start();
  void run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt);
  String get_name();
  
  void display_time();
  void display_cursor();
  void displayApps();
  
  void checkPress(const PointInt& pt);
};
