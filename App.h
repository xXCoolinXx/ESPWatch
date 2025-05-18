#pragma once
#include "Color.h"
#include <Arduino.h>
//#include "Kernel.h"

class Kernel;

//Common framework for apps so that Kernel can call them easily
//More akin to a Rust traits than a real class
class App {
  public:
  App(Kernel* kernel);
  virtual void run_code(double x, double y, bool special, Kernel* kernel)=0;
  virtual String get_name();
};

//New app copy and paste

/*  newApp.h 
#include "App.h"

class Kernel;

class newApp : public App
{
  public:
  newApp(Kernel* kernel);
  void run_code(double x, double y, bool special, Kernel* kernel);
  String get_name();
}  
 * newApp.cpp
#include "newApp.h"
#include "Kernel.h"

newApp::newApp(Kernel* kernel) : App(kernel) {}

void newApp::run_code(double x, double y, bool special, Kernel* kernel) {}

String newApp::get_name() {
  return String("newApp");
}
*/
