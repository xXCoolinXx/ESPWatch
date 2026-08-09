#ifndef KERNEL_H
#define KERNEL_H
//#include "App.h"

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1331.h>
// #include <FS.h>
// #include <LittleFS.h>
#include <deque>
#include <memory>
#include "src/config/Secrets.h"
#include "src/assets/fonts/NotoSansBold15.h"
#include "src/assets/fonts/NotoSansBold36.h"
#include "src/config/GFXConstants.h"
#include "src/config/Ports.h"
#include "src/kernel/TouchHandler.hpp"
#include "src/kernel/ClockDaemon.h"
#include <SensorQMI8658.hpp>
#include <optional>

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36

class App;

const float STEP_THRESHOLD = 1.25f;  // Trigger threshold in Gs (1.0 = resting gravity)
const float HYSTERESIS = 0.15f;       // Reset threshold band below peak
const float ALPHA = 0.25f;            // Low-pass filter factor (smooths noise)
const unsigned long MIN_STEP_MS = 280;

const double BATTERY_MAX_VOLT = 4.20;
const double BATTERY_MIN_VOLT = 3.0;
const float SLEEP_WHEN_BATTERY_INTERNAL_PCT = 0.20; 
const double BATTERY_VOLTS_EMA_ALPHA = 0.05;
const double BATTERY_USER_FLOOR = SLEEP_WHEN_BATTERY_INTERNAL_PCT * (BATTERY_MAX_VOLT - BATTERY_MIN_VOLT) + BATTERY_MIN_VOLT;
const double BATTERY_USER_RANGE = BATTERY_MAX_VOLT - BATTERY_USER_FLOOR;
const double BATTERY_CONVERSION_FACTOR = 3.0 / 1000.0;
const unsigned long SLEEP_AFTER_MILLIS = 90000;
// Not the displayed percentage, but percentage in the interval defined above

// Apps are identified by an ID so the Kernel can switch between them
// safely, instead of apps deleting themselves mid-run_code.
enum class AppId {
  NONE,
  START,
  PONG,
  SNAKE,
  NEWS
};

class Kernel {
  private:
  std::unique_ptr<App> current_app = nullptr;
  AppId current_app_id = AppId::NONE;
  AppId pending_app = AppId::NONE;
  std::deque<long> frame_times; 
  
  void setup_display();
  void init_wifi();
  void setup_qmi();

  void set_app(std::unique_ptr<App> app);
  void apply_pending_app();
  std::unique_ptr<App> create_app(AppId id);
  
  unsigned long lastTouchTime = millis();

  bool lastSpecial = false;

  bool clear_screen = false;
  
  std::unique_ptr<TFT_eSprite> start_sprite = nullptr;

  // Previous angles for analog clock hands
  double prev_a_hour = -1.0;
  double prev_a_min = -1.0;

  unsigned long deltaTime = 0;

  float filteredMagnitude = 1.0;
  unsigned long stepCount = 0;
  bool peakDetected = false;
  unsigned long lastStepTime = 0;

  std::optional<double> battery_volts;
  public: 
  TFT_eSPI display; // Pins are set up in the library for some reason. Bad practice but I can't fix it
  ClockDaemon _clock;
  TouchHandler::Handler touch;
  SensorQMI8658 qmi;  

  Kernel();
  ~Kernel();
  
  void setupf();
  void loopf();
  float get_fps(); 
  void request_app(AppId id);

  void clearNext(); //Clear on next loop
  void clearOnce(); //Check if screen is supposed to be cleared 

  // Unloads existing font
  void loadBigFont(); 
  void loadSmallFont();

  void drawAnalog();
  void drawAngledWideLine(int r1, int r2, double a, int w=1, uint16_t fg=TFT_WHITE, uint16_t bg=TFT_BLACK);

  void clearViewBox(uint16_t bg = TFT_BLACK); //Clear the square where programs besides the analog clock run

  void drawViewBox(uint16_t border=TFT_WHITE);

  void checkBattery(); 
  //Gives percentage level
  int getBatteryLevel();
  int getPodometerCount();
  void loop_podometer();

  double getDeltaTime();
};
#endif // KERNEL_H
