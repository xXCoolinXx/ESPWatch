#include "src/kernel/ClockDaemon.h"
#include "src/config/Ports.h"
#include "time.h"
// #undef SERIAL

void ClockDaemon::sync() {
  if(time(NULL) > 946684800) {
    // Time is already set, we don't need to sync
    // This is necessary to check when waking from deep sleep where the time was previously set
    return;
  }

  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println("Getting NTP time failed - WiFi not connected.");
    return; 
  }
  
  configTzTime(time_zone, ntpServer1, ntpServer2);

  if(!getLocalTime(&this->current_time)) {
    Serial.println("Getting time failed - :(");
    return;
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








