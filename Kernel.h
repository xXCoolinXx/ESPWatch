#pragma once
//#include "App.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1331.h>
// #include <FS.h>
// #include <LittleFS.h>
#include <deque>

class App;
class ClockDaemon;

class Kernel {
  private:
  App* current_app;
  std::deque<long> frame_times; 
  
  void setup_display();

  bool lastSpecial = false;

  bool clear_screen = false;
  
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
};
