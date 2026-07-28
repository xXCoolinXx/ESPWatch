#include "ClockDaemon.h"
#include "Ports.h"
#include "time.h"
// #undef SERIAL

void ClockDaemon::update_time_from_tm(struct tm &time_info) {
  this->tm.Second = time_info.tm_sec;
  this->tm.Minute = time_info.tm_min;
  this->tm.Hour   = time_info.tm_hour;
  this->tm.Day    = time_info.tm_mday;
  this->tm.Month  = time_info.tm_mon + 1;
  this->tm.Year   = time_info.tm_year - 70;         
  this->tm.Wday   = time_info.tm_wday + 1;         
}

void ClockDaemon::sync() {
  struct tm time_info;
  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println("Getting NTP time failed - WiFi not connected.");
    return; 
  }
  if(!getLocalTime(&time_info)) {
    Serial.println("Getting NTP time failed :(");
    return;
  } 

  this->update_time_from_tm(time_info);
}

void ClockDaemon::setupf() {
  sync();

  setSyncProvider(RTC.get);

  loadAlarms();

  update_time();
}

void ClockDaemon::update_time() {
  //last_tm = tm;
  //RTC.read(tm);
  last_tm = tm;
  breakTime(now(), tm);
}

void ClockDaemon::write_time(tmElements_t new_time) {
  RTC.write(new_time);
  update_time();
}

void ClockDaemon::loopf() {
  update_time();
  updateStopwatch();
  update_timer();
  updateAlarms();
}

tmElements_t ClockDaemon::get_time() {
  return tm;
}

time_t ClockDaemon::get_timet() {
  return makeTime(tm);
}

//Useful functions for updating displays
bool ClockDaemon::minuteChanged() {
 return last_tm.Minute != tm.Minute;
}
bool ClockDaemon::secondChanged() {
  return last_tm.Second != tm.Second;
}

void ClockDaemon::toggleStopwatch() {
  stopwatch_running = !stopwatch_running;
}

void ClockDaemon::updateStopwatch() {
  //This is a bit ugly but its far easier than how I was doing it originally
  if(secondChanged()) {
    stopwatch.Second += stopwatch_running; //Only update stopwatch if its running
    if(stopwatch.Second == 60) {
      stopwatch.Second = 0;
      stopwatch.Minute++;
      if(stopwatch.Minute == 60) {
        stopwatch.Minute = 0;
        stopwatch.Hour++;
      }
    }
  }
}

void ClockDaemon::clearStopwatch() {
  stopwatch.Hour = stopwatch.Minute = stopwatch.Second = 0;
  stopwatch_running = false;
}

bool ClockDaemon::stopwatchIsRunning() {
  return stopwatch_running;
}

tmElements_t ClockDaemon::getStopwatchTime() {
  return stopwatch;
}


void ClockDaemon::timerSecond(Add dir) {
  if(timer_running) return; //Leave function if the timer is currently running
  if (timer.Second > 0 || dir > 0) {
    timer.Second += dir;
  } else {
    timer.Second = 59;
  }
  if(timer.Second == 60) {
    timer.Second = 0;
  }
}

void ClockDaemon::timerMinute(Add dir) {
  if(timer_running) return; //Leave function if the timer is currently running
  if (timer.Minute > 0 || dir > 0) {
    timer.Minute += dir;
  } else {
    timer.Minute = 59;
  }
  if(timer.Minute == 60) {
    timer.Minute = 0;
  }
}

void ClockDaemon::timerHour(Add dir) {
  if(timer_running) return; //Leave function if the timer is currently running
  if(timer.Hour > 0 || dir > 0) {
    timer.Hour += dir;
  } else {
    timer.Hour = 99;
  }
  if(timer.Hour == 100) {
    timer.Hour = 0;
  }
}

void ClockDaemon::toggleTimer() {
  timer_running = !timer_running;
}

void ClockDaemon::clearTimer() {
  timer.Second = timer.Hour = timer.Minute = 0;
  timer_running = false;
}

void ClockDaemon::update_timer() {
  if(timer_running and secondChanged()) {
    if (timer.Second > 0) {
      timer.Second--;
    } else {
      if(timer.Minute > 0) {
        timer.Minute--;
        timer.Second = 59; 
      } else {
        if(timer.Hour > 0) {
          timer.Hour--;
          timer.Minute = 59;
          timer.Second = 59;
        } else {
          clearTimer();
          timerSound();
        }
      }
    }
  }
}

tmElements_t ClockDaemon::getTimerTime() {
  return timer;
}

bool ClockDaemon::timerRunning() {
  return timer_running;
}

void ClockDaemon::timerSound() {
  drip();
}

AlarmStruct* ClockDaemon::getAlarm(byte num) {
  return &alarms[num]; 
}

void ClockDaemon::updateAlarms() {
  for (int i = 0; i < NUM_ALARMS; i++) {
    alarms[i].check(get_time());
  }
}

void ClockDaemon::loadAlarms() {
  if(LittleFS.exists(ALARM_PATH)) {
    StaticJsonDocument<999> doc;
    
    File f = LittleFS.open(ALARM_PATH, "r");
    
    deserializeJson(doc, f);

    for(int i = 0; i < NUM_ALARMS; i++) {
      JsonObject data = doc[String(i)];
      alarms[i].Hour = data["Hour"];
      alarms[i].Minute = data["Minute"];
      alarms[i].Set = data["Set"];

      JsonArray arr = data["Days"];
      for(JsonVariant v : arr) {
        alarms[i].Days.insert(static_cast<uint8_t>(v));
      }
    } 
  }
}

void ClockDaemon::writeAlarms() {
  StaticJsonDocument<999> doc;
  for(int i = 0; i < NUM_ALARMS; i++) {
    JsonObject data = doc.createNestedObject(String(i));
    data["Hour"] = alarms[i].Hour;
    data["Minute"] = alarms[i].Minute;
    data["Set"] = alarms[i].Set;
    JsonArray wdays = data.createNestedArray("Days");
    for(int j = 1; j <= 7; j++) {
      if(alarms[i].Days.find(j) != alarms[i].Days.end()) {
        wdays.add(j);
      }
    }
    //{
    //  ...
    //  : {
    //    Hour : ,
    //    Minute : ,
    //    Set : ,
    //    Days : [ , , ..., ]
    //  }
    //  ...
    //}
  }
  
  File f = LittleFS.open(ALARM_PATH, "w"); 
  
  serializeJson(doc, f);
}
