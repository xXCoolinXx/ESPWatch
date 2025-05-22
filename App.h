#pragma once
#include "Color.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
//#include "Kernel.h"

class Kernel;

//Common framework for apps so that Kernel can call them easily
//More akin to a Rust traits than a real class
class App {
  virtual void _setup_sprites();
  public:
  App(Kernel* kernel);
  virtual void run_code(double x, double y, bool special)=0;
  virtual String get_name() = 0;
};

// Utility function to make sprites easily
void make_sprite(TFT_eSprite*& spr, TFT_eSPI* display, int width, int height, const uint16_t bitmap[]);

//New app copy and paste

/*  newApp.h 
#include "App.h"

class Kernel;

class newApp : public App
{
  Kernel* kernel;
  public:
  newApp(Kernel* kernel);
  void run_code(double x, double y, bool special, Kernel* kernel);
  String get_name();
}  
 * newApp.cpp
#include "newApp.h"
#include "Kernel.h"

newApp::newApp(Kernel* kernel) : App(kernel) {
  this->kernel = kernel;
}

void newApp::run_code(double x, double y, bool special, Kernel* kernel) {}

String newApp::get_name() {
  return String("newApp");
}
*/
