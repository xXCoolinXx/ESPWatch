#ifndef CLOCK_DAEMON_H
#define CLOCK_DAEMON_H
#include <WiFi.h> // for WiFi shield

// #include <TimeLib.h>
#include <time.h>
// #include <TimeAlarms.h>
// #include <FS.h>
// #include <LittleFS.h>
// #include <RTClib.h>

#define NUM_ALARMS 3
#define ALARM_PATH "/alarms.json"

// NTP Setup
inline constexpr const char* ntpServer1 = "pool.ntp.org";
inline constexpr const char* ntpServer2 = "time.nist.gov";

inline constexpr const char* time_zone = "CST6CDT,M3.2.0,M11.1.0";

enum Add {
  DOWN = -1,
  UP = 1
};

class Kernel; // Define kernel class to avoid circular imports

class ClockDaemon {
  private:
    struct tm stopwatch; //Only hour, minute, and second are used
    struct tm current_time;
    struct tm previous_time;

    bool stopwatch_running = false;

    Kernel& kernel;
  public:
    ClockDaemon(Kernel& kernel);

    void sync();
    void setupf();
 
    void update_time();
    void write_time(struct tm new_time);
    struct tm get_time();
    time_t get_timet();

    bool minuteChanged();
    bool secondChanged();

    void loopf();

    void toggleStopwatch();
    void clearStopwatch();
    void updateStopwatch();
    bool stopwatchIsRunning();
    struct tm getStopwatchTime();
};
#endif //
