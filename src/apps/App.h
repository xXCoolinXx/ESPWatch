#ifndef APP_H
#define APP_H
#include "src/utils/Color.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
//#include "Kernel.h"
#include "src/kernel/TouchHandler.hpp"
#include <memory>
class Kernel;

//Common framework for apps so that Kernel can call them easily
//More akin to a Rust traits than a real class
class App {
  protected:
  Kernel& kernel; // Non-owning reference to the Kernel that owns this app (outlives every app)
  virtual void _setup_sprites();
  public:
  App(Kernel& kernel);
  // Apps are deleted through App* (Kernel::set_app), so the destructor MUST be virtual
  virtual ~App() = default;
  virtual void run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt)=0;
  virtual String get_name() = 0;
};

// Utility function to make sprites easily. Returns nullptr on allocation failure.
std::unique_ptr<TFT_eSprite> make_sprite(TFT_eSPI* display, int width, int height, const uint16_t bitmap[]);

/* New app template (copy these into a new MyApp.h / MyApp.cpp):

   // MyApp.h
   #include "src/apps/App.h"

   class MyApp : public App {
   public:
     MyApp(Kernel& kernel); // Base class stores kernel for you (protected reference)
     void run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt) override;
     String get_name() override;
   };

   // MyApp.cpp
   #include "MyApp.h"

   MyApp::MyApp(Kernel& kernel) : App(kernel) {
     // this->kernel is ready to use here
   }

   void MyApp::run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt) {}

   String MyApp::get_name() { return String("MyApp"); }

   // Register it in Kernel::create_app() so the launcher can switch to it.
*/
#endif // APP_H
