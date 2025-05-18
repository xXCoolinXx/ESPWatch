#pragma once
// #include <DS1307RTC.h>
#include <NTPClient.h>
// change next line to use with another board/shield
// #include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
// #include <FS.h>
// #include <LittleFS.h>
#include <ArduinoJson.h>
#include <bits/stdc++.h>
#include <RTClib.h>

#define NUM_ALARMS 3
#define ALARM_PATH "/alarms.json"

void drip();

enum Add {
  DOWN = -1,
  UP = 1
};

struct AlarmStruct {
  uint8_t Hour = 0;
  uint8_t Minute = 0;
  std::unordered_set<uint8_t> Days;
  bool Set = false;
  bool triggered = false; 

  void check(tmElements_t tm) {
    if (!Set) return; //Only check if the alarm is set
    if(tm.Hour == Hour and tm.Minute == Minute and Days.find(tm.Wday) != Days.end()) { 
      if(not triggered) {
        triggered = true;
        drip(); 
      }
    } else {
      triggered = false;
    }
  }
};

class ClockDaemon {
  private:
    tmElements_t stopwatch; //Only hour, minute, and second are used
    tmElements_t timer; //Same deal
    
    tmElements_t tm;
    tmElements_t last_tm;

    bool stopwatch_running = false;
    bool timer_running = false;

    AlarmStruct alarms[NUM_ALARMS];

    RTC_DS1307 clock;

  public:
    //ClockDaemon();

    void sync();
    void setupf();
    
    void update_time();
    void write_time(tmElements_t new_time);
    tmElements_t get_time();
    time_t get_timet();

    bool minuteChanged();
    bool secondChanged();

    void loopf();

    void toggleStopwatch();
    void clearStopwatch();
    void updateStopwatch();
    bool stopwatchIsRunning();
    tmElements_t getStopwatchTime();

    void timerSecond(Add dir);
    void timerMinute(Add dir);
    void timerHour  (Add dir);
    void toggleTimer();
    void clearTimer ();
    void timerSound();
    tmElements_t getTimerTime();
    bool timerRunning();
    void update_timer();

    void loadAlarms();
    void writeAlarms();
    void updateAlarms();
    AlarmStruct* getAlarm(byte num);
};
