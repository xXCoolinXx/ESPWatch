#pragma once
#include "App.h"
#include "Shapes.h"

#define APP_DIM 18
#define OUTLINE_DIM 20
#define APP_SPACING_X 9 //96 - 3 * OUTLINE_DIM / 4
#define APP_SPACING_Y 4
#define APP_START 12
#define ROWS 2
#define COLS 3

//What no list comprehension do to a mf
const RectInt app_rects[ROWS*COLS] = {
   RectInt{APP_SPACING_X * (0 + 1) + OUTLINE_DIM * 0 + 1, APP_START + APP_SPACING_Y * (0 + 1) + OUTLINE_DIM*0 + 1, APP_DIM, APP_DIM},
   RectInt{APP_SPACING_X * (1 + 1) + OUTLINE_DIM * 1 + 1, APP_START + APP_SPACING_Y * (0 + 1) + OUTLINE_DIM*0 + 1, APP_DIM, APP_DIM},
   RectInt{APP_SPACING_X * (2 + 1) + OUTLINE_DIM * 2 + 1, APP_START + APP_SPACING_Y * (0 + 1) + OUTLINE_DIM*0 + 1, APP_DIM, APP_DIM},
   RectInt{APP_SPACING_X * (0 + 1) + OUTLINE_DIM * 0 + 1, APP_START + APP_SPACING_Y * (1 + 1) + OUTLINE_DIM*1 + 1, APP_DIM, APP_DIM},
   RectInt{APP_SPACING_X * (1 + 1) + OUTLINE_DIM * 1 + 1, APP_START + APP_SPACING_Y * (1 + 1) + OUTLINE_DIM*1 + 1, APP_DIM, APP_DIM},
   RectInt{APP_SPACING_X * (2 + 1) + OUTLINE_DIM * 2 + 1, APP_START + APP_SPACING_Y * (1 + 1) + OUTLINE_DIM*1 + 1, APP_DIM, APP_DIM},
};

class Start : public App {
  private: 
    RectDouble Cursor;
    RectDouble lastCursor;
  public:
  Start(Kernel* kernel);
  void run_code(double x, double y, bool special, Kernel* kernel);
  String get_name();
  
  void display_time(Kernel* kernel);
  void display_cursor(Kernel* kernel);
  void displayApps(Kernel* kernel);
  
  void move_cursor(double x, double y, Kernel* kernel);
  void checkPress(bool special, Kernel* kernel);
};
