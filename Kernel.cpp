#include "Kernel.h"
#include "Joystick.h"
#include "Ports.h"
#include "Start.h"
#include "ClockDaemon.h"
#include "Print.h"
#include "Images.h"
#include <numeric>

//#define DEBUG
const int frame_count = 10;

Kernel::Kernel() : display(TFT_eSPI()) {
}

void Kernel::setup_display() {
  display.begin(); 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.fillScreen(WHITE); 
  display.drawRGBBitmap(24, 8, arduino, 48, 48);
}

void Kernel::setupf() {
  setup_display();
  setup_joystick(VRx, VRy, SW);
  // LittleFS.begin();  

  pinMode(boggle, INPUT_PULLUP);

  set_app(new Start(this));

  _clock = new ClockDaemon;
  _clock->setupf();

  randomSeed(_clock->get_timet());
}

void Kernel::loopf() {
  long t_0 = millis();
  
  _clock->looppf();

  clearOnce(); 
  
   display.setCursor(0, 0);
   display.setTextColor(WHITE);

  bool currentSpecial = !digitalRead(SW); //Inverted because it is a pullup pin
   current_app->run_code(
    conform_joystick_axis(VRx, ANALOG, 25, 526, 750), 
    conform_joystick_axis(VRy, ANALOG, 75, 641, 880), 
    currentSpecial,
    this);

  #ifdef DEBUG
  display.setCursor(0, display.height() - 7);
  display.fillRect(30, display.height() - 7, 50, 7, BLACK);
  display.print(String("FPS: ") + get_fps() + " ");
  display.print(digitalRead(boggle));
  #endif
  
  //display.fillScreen(BLACK); //REMOVE THIS LATER!
  lastSpecial = currentSpecial;
  checkBoggle();

   long elapsed = millis() - t_0;
  if (frame_times.size() == frame_count) {
    frame_times.pop_front();
  }
  frame_times.push_back(elapsed); 
}

//Takes the last 10 frames to see the FPS
float Kernel::get_fps() {
  long sum = std::accumulate(frame_times.begin(), frame_times.end(), 0);
  return 1000 * frame_count / (sum + 0.00);
}

void Kernel::set_app(App* app) {
  delete current_app;
  current_app = app;
  clearNext();
}

bool Kernel::getLastSpecial() {
  return lastSpecial;
}

void Kernel::checkBoggle() {
  if(!digitalRead(boggle) && current_app->get_name() != "Start") {
    set_app(new Start(this));
  }
}

void Kernel::clearNext() {
  clear_screen = true;
}

void Kernel::clearOnce() {
  if (clear_screen) {
    display.fillScreen(BLACK);
    clear_screen = false;
  }
}
