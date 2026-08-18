#include "src/kernel/Kernel.h"
#include "src/config/Ports.h"
#include "src/apps/Start.h"
#include "src/apps/Pong.h"
#include "src/apps/Snake.h"
#include "src/apps/News.h"
#include "src/kernel/ClockDaemon.h"
#include "src/utils/MyPrint.h"
#include "src/assets/Images.h"
#include <numeric>
#include <Wire.h>
#include "src/apps/App.h"
#include "src/utils/Shapes.h"
#include "Battery.hpp"

//#define DEBUG

void Kernel::handle_sleep() {
  if( (millis() - this->lastTouchTime) > SLEEP_AFTER_MILLIS) {
    // Disable backlight
    digitalWrite(TFT_BL, LOW); 
    gpio_hold_en((gpio_num_t)TFT_BL);
    
    // pinMode(TP_INT, INPUT_PULLUP);
    // esp_sleep_enable_ext0_wakeup((gpio_num_t)TP_INT, 0);
    // // gpio_wakeup_enable((gpio_num_t)TP_INT, GPIO_INTR_LOW_LEVEL);
    // esp_sleep_enable_gpio_wakeup();
    
    Serial.println("Attempting to enter sleep");
    Serial.flush();
    while (this->touch.available()) {
      delay(10);
    }

    gpio_wakeup_enable((gpio_num_t)TP_INT, GPIO_INTR_LOW_LEVEL);
  
    // 3. ENABLE GPIO WAKE FOR LIGHT SLEEP
    // This is the critical step that tells the ESP32-S3 to honor gpio_wakeup_enable during light sleep.
    esp_sleep_enable_gpio_wakeup();
    
    Serial.println("Sleeping now");
    Serial.flush();
    // 4. ENTER LIGHT SLEEP
    esp_light_sleep_start();
  
    // --- The ESP32 wakes up here when you touch the screen ---
  
    // 5. DISABLE WAKE SOURCE (Optional, but good practice)
    gpio_wakeup_disable((gpio_num_t)TP_INT);
    
    gpio_hold_dis((gpio_num_t)TFT_BL); // Release from sleep cycle
    digitalWrite(TFT_BL, HIGH);
    Serial.println("Awoke from sleep, woke af");
    this->lastTouchTime = millis();
    //esp_light_sleep_start();
  
  } 
}

bool Kernel::request_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD); // Define in Secrets.h

  Serial.print("Connecting to WiFi ..");
  int checks = 0;
  while (WiFi.status() != WL_CONNECTED && checks < 20) {
    Serial.print('.');
    checks++;
    delay(5000);
  }

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi failed to connect :(");

    return false;
  } else { 
    Serial.println(WiFi.localIP());
    return true;
  }
}

void Kernel::turn_off_wifi() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}


void Kernel::setup_qmi() {
  if(!this->qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, TP_SDA, TP_SCL)) {
    Serial.println("Woops, accelerometer didn't start :(");
    return; 
  }

  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_62_5Hz);
  qmi.enableAccelerometer();
  
  qmi.configPedometer(
        50,   // ped_sample_cnt: Window sampl, _clock(this)  count
        200,  // ped_peak2peak:  Peak-to-peak threshold
        100,  // ped_peak:       Peak detection threshold
        200,  // ped_time_up:    Max time for step up-slope (ms)
        20,   // ped_time_low:   Min time between steps (ms)
        10,   // ped_entry_cnt:  Initial step requirement before counting starts (e.g. 10 steps)
        0,    // ped_precision:  Precision level
        4     // ped_sig_count:  Update output register every N steps
  );

  qmi.enablePedometer();
  qmi.clearPedometerCounter();
}

void Kernel::loop_podometer() {
  if (qmi.getDataReady()) {
        IMUdata accel;
        qmi.getAccelerometer(accel.x, accel.y, accel.z);

        // 1. Calculate vector magnitude (independent of board orientation)
        float rawMagnitude = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);

        // 2. Low-pass filter to smooth out high-frequency noise
        filteredMagnitude = (ALPHA * rawMagnitude) + ((1.0f - ALPHA) * filteredMagnitude);

        // 3. Peak detection
        unsigned long now = millis();
        if (filteredMagnitude > STEP_THRESHOLD && !peakDetected) {
            if (now - lastStepTime > MIN_STEP_MS) {
                stepCount++;
                lastStepTime = now;
                peakDetected = true;
                
                Serial.print("Step Count: ");
                Serial.println(stepCount);
            }
        } 
        // Reset peak flag when acceleration drops back down
        else if (filteredMagnitude < (STEP_THRESHOLD - HYSTERESIS)) {
            peakDetected = false;
        }
    }
}

Kernel::Kernel() : display(TFT_eSPI()), _clock(*this) {
  this->current_app = nullptr;
}

// Defined here (not inline in the header) so std::unique_ptr<App> can be
// destroyed where App is a complete type.
Kernel::~Kernel() = default;

void Kernel::setup_display() {
  gpio_hold_dis((gpio_num_t)TFT_BL); // Release from sleep cycle
  
  display.begin(); 
  display.setTextSize(1);
  display.setRotation(1);
  display.setTextColor(WHITE);
  
  display.fillScreen(WHITE); 
  // display.drawBitmap(24, 8, arduino, 48, 48);

  this->start_sprite = make_sprite(&(this->display), 48, 48, arduino);

  if(this->start_sprite) {
    int x = (screen_width - this->start_sprite->width()) / 2;
    int y = (screen_height - this->start_sprite->height()) / 2;
    this->start_sprite->pushSprite(x, y);
  }
}

void Kernel::setupf() {
  Serial.begin(115200);
  
  // esp_sleep_enable_gpio_wakeup();
  Serial.println("Reached Kernel::setupf");
  
  this->setup_display();
  this->touch.init();
  this->setup_qmi();
  Serial.println("Setting up display works!");
  // LittleFS.begin();  
  // pinMode(SW, INPUT_PULLUP);
  // pinMode(boggle, INPUT_PULLUP);
  
  _clock.setupf();

  set_app(create_app(AppId::START));
  current_app_id = AppId::START;

  randomSeed(_clock.get_timet());

  // this->loadBigFont();
  // this->loadSmallFont();
  this->display.setTextColor(TFT_WHITE, TFT_BLACK);
  
  this->lastTouchTime = millis();
  
  gpio_set_direction((gpio_num_t)TP_INT, GPIO_MODE_INPUT);
  gpio_pullup_en((gpio_num_t)TP_INT);

  Serial.println("Finished setup");
}

void Kernel::loopf() {
  long t_0 = millis();
  
  this->_clock.loopf();
  // this->loop_podometer(); // Remove this because the podometer just isn't very useful without long term tracking, which is a pain to implement and my phone already does this
  this->checkBattery();

  clearOnce(); 
  
  // Always draws the Analog watch on the screen
  this->drawAnalog();
  
  // Resets the cursor so stuff doesn't get fucked up
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
    
  auto [gesture, current_point] = this->touch.getTouchData();
  
  // Check if either has a value
  if(gesture.has_value() || current_point.has_value()) {
    this->lastTouchTime = millis();
  } 
  
  this->handle_sleep();

  if(gesture.has_value() && gesture.value() == TouchHandler::Gesture::DOWN_SLASH) {
    request_app(AppId::START);
  } 
  
  current_app->run_code(gesture, current_point);
  apply_pending_app();

  #ifdef DEBUG
  display.setCursor(0, display.height() - 7);
  display.fillRect(30, display.height() - 7, 50, 7, BLACK);
  display.print(String("FPS: ") + get_fps() + " ");
  display.print(digitalRead(boggle));
  #endif
  

  this->deltaTime = millis() - t_0;
  if (frame_times.size() == frame_count) {
    frame_times.pop_front();
  }
  frame_times.push_back(deltaTime); 
}

//Takes the last 10 frames to see the FPS
float Kernel::get_fps() {
  long sum = std::accumulate(frame_times.begin(), frame_times.end(), 0);
  return 1000 * frame_count / (sum + 0.00);
}

void Kernel::set_app(std::unique_ptr<App> app) {
  // Replacing current_app destroys the previous app (smart pointer cleanup)
  current_app = std::move(app);
  clearNext();
}

void Kernel::request_app(AppId id) {
  turn_off_wifi(); // Garbage collect wifi usage after a program has run
  if(id != AppId::NONE) {
    pending_app = id;
  }
}

std::unique_ptr<App> Kernel::create_app(AppId id) {
  turn_off_wifi(); // Turn off wifi before switching apps to avoid keeping it on too long
  switch (id) {
    case AppId::PONG:  return std::make_unique<Pong>(*this);
    case AppId::SNAKE: return std::make_unique<Snake>(*this);
    case AppId::NEWS:  return std::make_unique<News>(*this);
    case AppId::START:
    default:           return std::make_unique<Start>(*this);
  }
}

// App switches happen here, AFTER the current app's run_code has returned.
// Apps must never delete themselves mid-run_code (that was a use-after-free).
void Kernel::apply_pending_app() {
  if(pending_app == AppId::NONE || pending_app == current_app_id) {
    pending_app = AppId::NONE;
    return;
  }

  AppId id = pending_app;
  pending_app = AppId::NONE;
  set_app(create_app(id));
  current_app_id = id;
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

void Kernel::loadBigFont() {
  this->display.unloadFont();
  this->display.loadFont(AA_FONT_LARGE);
}

void Kernel::loadSmallFont() {
  this->display.unloadFont();
  this->display.loadFont(AA_FONT_SMALL);
}

double theta_major(double k) { return 2 * M_PI * k / 12 - M_PI_2; } // 12 hours in 2 PI rad 
double theta_minor(double k) { return 2 * M_PI * k / 60 - M_PI_2; } // 60 minutes in 2 PI rad

void Kernel::drawAngledWideLine(int r1, int r2, double a, int w, uint16_t fg, uint16_t bg) {   
  auto x0 = screen_width/2.0 + r1 * cos(a);
  auto y0 = screen_height/2.0 + r1 * sin(a);
  auto x1 = screen_width/2.0 + r2 * cos(a);
  auto y1 = screen_height/2.0 + r2 * sin(a);

  this->display.drawWideLine(x0, y0, x1, y1, w, fg, bg);
}

void Kernel::drawAnalog() {
  for(int i = 0; i < 12; i++) {
    this->drawAngledWideLine(r, r_major, theta_major(i));
  }

  for(int i = 0; i < 60; i++) {
    this->drawAngledWideLine(r, r_minor, theta_minor(i));
  }

  // drawWedgeLine(float ax, float ay, float bx, float by, float aw, float bw, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF)
  auto tm  = this->_clock.get_time();

  double a_hour = theta_major(tm.tm_hour%12 + tm.tm_min / 60.0);
  double a_min = theta_minor(tm.tm_min + tm.tm_sec/60.0);

  // Serial.println(tm.Minute);

  if(this->prev_a_hour != -1.0 && (this->prev_a_hour != a_hour || this->prev_a_min != a_min)) {
    this->drawAngledWideLine(r_cursor, r_cursor-10, this->prev_a_hour, cursor_w+2, TFT_BLACK, TFT_BLACK);
    this->drawAngledWideLine(r_cursor, r_cursor-10, this->prev_a_min, cursor_w+2, TFT_BLACK, TFT_BLACK);
  }

  this->drawAngledWideLine(r_cursor, r_cursor-10, a_hour, cursor_w, (tm.tm_hour > 12) ? TFT_BLUE : TFT_YELLOW);
  this->drawAngledWideLine(r_cursor, r_cursor-10, a_min, cursor_w);
  
  this->prev_a_hour = a_hour;
  this->prev_a_min = a_min;
  // this->display.drawWedgeLine( , TFT_WHITE, TFT_BLACK);
}

void Kernel::clearViewBox(uint16_t bg) {
  this->display.fillRect(left_vb, top_vb, viewbox_wh, viewbox_wh, bg);
}

void Kernel::drawViewBox(uint16_t border) {
  this->display.drawRect(left_vb, top_vb, viewbox_wh, viewbox_wh, border);
}

void Kernel::checkBattery() {
  // double new_volts = analogReadMilliVolts(BAT_ADC_PIN) * 3.3f / (1 << 12) * 3;

  double new_volts = analogReadMilliVolts(BAT_ADC_PIN) * BATTERY_CONVERSION_FACTOR;

  // Handle optional-ness
  if(!battery_volts.has_value()) {
    battery_volts = new_volts;
  } else {
    this->battery_volts = BATTERY_VOLTS_EMA_ALPHA * new_volts + (1 - BATTERY_VOLTS_EMA_ALPHA) * this->battery_volts.value();
  }

  if(this->battery_volts.value() < BATTERY_USER_FLOOR) {
    // TODO: Sleep
  }
}

int Kernel::getBatteryLevel() {
  if(!this->battery_volts.has_value()) {
    return 0;
  }
  
  return stateOfChargePercent(this->battery_volts.value()); 

  // return round(
  //     (min(BATTERY_MAX_VOLT, this->battery_volts.value()) - BATTERY_USER_FLOOR) / BATTERY_USER_RANGE
  //   * 100); 
}

int Kernel::getPodometerCount() {
  return this->stepCount;
}

double Kernel::getDeltaTime() {
  return double(this->deltaTime) / 60.0;
}
