#include "MyPrint.h"
#include "Color.h"

void print2(short number, TFT_eSPI* display) {
  //If it is less than 10, print a 0
  if (number >= 0 && number < 10) {
    display->print(0);
  }
  //print the number
  display->print(number);
}

void print2_space(short number, TFT_eSPI* display) {
  if (number >= 0 && number < 10) {
    display->print(" ");
  }
  display->print(number);
}

void print_time(tmElements_t tm, TFT_eSPI* display, int x, int y) {
  // auto colon = [display] { display->print(":"); };
  // print2(tm.Hour, display);
  // colon();
  // print2(tm.Minute, display);
  // colon();
  // print2(tm.Second, display);

  String out = format0(tm.Hour) + ":" + format0(tm.Minute);
  
  display->drawString(out, x, y);
}

void clear_chars(short x, short y, short num_chars, TFT_eSPI* display) {
  display->fillRect(x, y, num_chars * CHAR_SPACE_W, CHAR_HEIGHT, BLACK);
}

String format0(short number) {
  if(number < 10) {
    return String("0" + String(number));
  } else {
    return String(number);
  }
}