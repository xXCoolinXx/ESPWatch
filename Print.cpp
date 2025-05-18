#include "Print.h"
#include "Color.h"

void print2(short number, Adafruit_SSD1331* display) {
  //If it is less than 10, print a 0
  if (number >= 0 && number < 10) {
    display->print(0);
  }
  //print the number
  display->print(number);
}

void print2_space(short number, Adafruit_SSD1331* display) {
  if (number >= 0 && number < 10) {
    display->print(" ");
  }
  display->print(number);
}

void print_time(tmElements_t tm, Adafruit_SSD1331* display) {
  auto colon = [display] { display->print(":"); };
  print2(tm.Hour, display);
  colon();
  print2(tm.Minute, display);
  colon();
  print2(tm.Second, display);
}

void clear_chars(short x, short y, short num_chars, Adafruit_SSD1331* display) {
  display->fillRect(x, y, num_chars * CHAR_SPACE_W, CHAR_HEIGHT, BLACK);
}
