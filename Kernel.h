#pragma once
//#include "App.h"
#define DS1307LIB_NOWIRE // Added to ensure DS1307 doesnt begin the wire for us // Doesn't work ...

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1331.h>
#include <FS.h>
#include <LittleFS.h>
#include <deque>
#include "Secrets.h"
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
#include "GFXConstants.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36

class App;
class ClockDaemon;

class Kernel {
  private:
  App* current_app = nullptr;
  std::deque<long> frame_times; 
  
  void setup_display();
  void init_wifi();

  bool lastSpecial = false;

  bool clear_screen = false;
  
  TFT_eSprite* start_sprite = nullptr;

  // Previous angles for analog clock hands
  double prev_a_hour = -1.0;
  double prev_a_min = -1.0;

  unsigned long deltaTime = 0;
  public: 
  TFT_eSPI display; // Pins are set up in the library for some reason. Bad practice but I can't fix it
  ClockDaemon* _clock;

  Kernel();
  
  void setupf();
  void loopf();
  float get_fps(); 
  void set_app(App* app);

  bool getLastSpecial();
  void checkBoggle();

  void clearNext(); //Clear on next loop
  void clearOnce(); //Check if screen is supposed to be cleared 

  // Unloads existing font
  void loadBigFont(); 
  void loadSmallFont();

  void drawAnalog();
  void drawAngledWideLine(int r1, int r2, double a, int w=1, uint16_t fg=TFT_WHITE, uint16_t bg=TFT_BLACK);

  void clearViewBox(uint16_t bg = TFT_BLACK); //Clear the square where programs besides the analog clock run

  void drawViewBox(uint16_t border=TFT_WHITE);

  double getDeltaTime();
};
