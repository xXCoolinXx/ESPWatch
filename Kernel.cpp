#include "Kernel.h"
#include "Ports.h"
#include "Start.h"
#include "ClockDaemon.h"
#include "MyPrint.h"
#include "Images.h"
#include <numeric>
#include <Wire.h>
#include "Shapes.h"

//#define DEBUG

void Kernel::init_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD); // Define in Secrets.h

  Serial.print("Connecting to WiFi ..");
  int checks = 0;
  while (WiFi.status() != WL_CONNECTED && checks < 20) {
    Serial.print('.');
    checks++;
    delay(1000);
  }

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi failed to connect :(");
  } else { 
    Serial.println(WiFi.localIP());
  }
}

void Kernel::init_touch() {
  touch.setPins(TP_RST, TP_INT);

  touch.setTouchDrvModel(TouchDrv_CST8XX);
  
  this->touch.begin(Wire, CST816_SLAVE_ADDRESS, TP_SDA, TP_SCL);  
}

Kernel::Kernel() : display(TFT_eSPI()) {
}

void Kernel::setup_display() {
  display.begin(); 
  display.setTextSize(1);
  display.setRotation(1);
  display.setTextColor(WHITE);
  
  display.fillScreen(WHITE); 
  // display.drawBitmap(24, 8, arduino, 48, 48);

  make_sprite(this->start_sprite, &(this->display), 48, 48, arduino);

  int x = (screen_width - this->start_sprite->width()) / 2;
  int y = (screen_height - this->start_sprite->height()) / 2;
  this->start_sprite->pushSprite(x, y);
}

void Kernel::setupf() {
  Serial.begin(115200);

  delay(100);                    // important on S3 USB-CDC
  Serial.println("Reached Kernel::setupf");
  
  this->setup_display();
  this->init_touch();
  this->init_wifi();
  Serial.println("Setting up display works!");
  // LittleFS.begin();  
  // pinMode(SW, INPUT_PULLUP);
  // pinMode(boggle, INPUT_PULLUP);

  set_app(new Start(this));

  _clock = new ClockDaemon;
  _clock->setupf();

  randomSeed(_clock->get_timet());

  // this->loadBigFont();
  // this->loadSmallFont();
  this->display.setTextColor(TFT_WHITE, TFT_BLACK);

  Serial.println("Finished setup");
}

void Kernel::loopf() {
  long t_0 = millis();
  
  _clock->loopf();

  clearOnce(); 
  
  // Always draws the Analog watch on the screen
  this->drawAnalog();
  
  // Resets the cursor so stuff doesn't get fucked up
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  
  TouchPoints tp = this->touch.getTouchPoints();

  current_app->run_code(tp);

  #ifdef DEBUG
  display.setCursor(0, display.height() - 7);
  display.fillRect(30, display.height() - 7, 50, 7, BLACK);
  display.print(String("FPS: ") + get_fps() + " ");
  display.print(digitalRead(boggle));
  #endif
  
  //display.fillScreen(BLACK); //REMOVE THIS LATER!

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

void Kernel::set_app(App* app) {
  if(current_app) {
    delete current_app;
  }
  current_app = app;
  // Serial.println((int)app);
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
  auto tm  = this->_clock->get_time();

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

double Kernel::getDeltaTime() {
  return double(this->deltaTime) / 60.0;
}
