#include "src/kernel/ClockDaemon.h"
#include "src/config/Ports.h"
#include "time.h"
#include "Kernel.h"
// #undef SERIAL

ClockDaemon::ClockDaemon(Kernel& kernel) : kernel(kernel) {

}

void ClockDaemon::sync() {
  // call regardless to set time zone - don't need wifi
  configTzTime(time_zone, ntpServer1, ntpServer2);


  if(time(NULL) > 946684800) {
    // Time is already set, we don't need to sync
    // This is necessary to check when waking from deep sleep where the time was previously set
    return;
  }
  
  struct tm timeinfo;
  if(this->kernel.request_wifi()) {
    Serial.println("WiFi for NTP!");
    if(getLocalTime(&timeinfo, 5000)) {
      Serial.println("Time succesfully synced!");
    } else {
      Serial.println("Time failed to sync... oops");
    }
  } else { 
    Serial.println("Getting time failed - :(");
  }
}

void ClockDaemon::setupf() {
  sync();

  update_time();
}

void ClockDaemon::update_time() {
  this->previous_time = current_time;
  getLocalTime(&this->current_time);
}

void ClockDaemon::loopf() {
  update_time();
  updateStopwatch();
}

struct tm ClockDaemon::get_time() {
  return this->current_time;
}

time_t ClockDaemon::get_timet() {
  return mktime(&this->current_time);
}

//Useful functions for updating displays
bool ClockDaemon::minuteChanged() {
  return this->previous_time.tm_min != this->current_time.tm_min;
}
bool ClockDaemon::secondChanged() {
  return this->previous_time.tm_min != this->current_time.tm_min;
}

void ClockDaemon::toggleStopwatch() {
  stopwatch_running = !stopwatch_running;
}

void ClockDaemon::updateStopwatch() {
  //This is a bit ugly but its far easier than how I was doing it originally
  if(secondChanged()) {
    stopwatch.tm_sec += stopwatch_running; //Only update stopwatch if its running
    if(stopwatch.tm_sec == 60) {
      stopwatch.tm_sec = 0;
      stopwatch.tm_min++;
      if(stopwatch.tm_min == 60) {
        stopwatch.tm_min = 0;
        stopwatch.tm_hour++;
      }
    }
  }
}

void ClockDaemon::clearStopwatch() {
  stopwatch.tm_hour = 0;
  stopwatch.tm_min = 0;
  stopwatch.tm_sec = 0;
  stopwatch_running = false;
}

bool ClockDaemon::stopwatchIsRunning() {
  return stopwatch_running;
}

struct tm ClockDaemon::getStopwatchTime() {
  return stopwatch;
}








