#include "Clock.h"
#include "Kernel.h"
#include "Images.h"
#include "ClockDaemon.h"
#include <TimeLib.h>

Selection operator++(Selection& s)
{
  if ( (s + ROW_SPACE) <= NUM_ROWS * ROW_SPACE) {
    s = static_cast<Selection>(s + ROW_SPACE);
  } else {
    s = STOPWATCH;
  }
  return s;
}

Selection operator--(Selection& s)
{
  if ( (s - ROW_SPACE) != 0) {
    s = static_cast<Selection>(s - ROW_SPACE);
  } else {
    s = ALARMS;
  }
  return s;
}

TimeSelection operator++(TimeSelection& s) {
  switch (s) {
    case NOT_EDITING:
      s = HOUR;
      break;
    case HOUR:
      s = MINUTE;
      break;
    case MINUTE:
      s = SECOND;
      break;
    case SECOND:
      s = HOUR;
      break;
  }
  return s;
}

TimeSelection operator--(TimeSelection& s) {
  switch (s) {
    case NOT_EDITING:
      s = NOT_EDITING;
      break;
    case HOUR:
      s = SECOND;
      break;
    case MINUTE:
      s = HOUR;
      break;
    case SECOND:
      s = MINUTE;
      break;
  }
  return s;
}

AlarmEdit operator++(AlarmEdit& s) {
  short new_s = static_cast<short>(s) + 1;
  if (new_s == 12 or new_s == 13) new_s = 1; //If goes to NOT or was NOT
  s = static_cast<AlarmEdit>(new_s);
  return s;
}

AlarmEdit operator--(AlarmEdit& s) {
  short new_s = static_cast<short>(s) - 1;
  if (new_s == 0) { new_s = 11; }
  s = static_cast<AlarmEdit>(new_s);
  return s;
}

short getX(AlarmEdit a) {
  switch (a) {
    case ID:
      return X_SPACE + 6 * CHAR_SPACE_W;
      break;
    case SET:
      return X_SPACE + 10 * CHAR_SPACE_W;
      break;
    case HOUR_:
      return X_SPACE + CHAR_SPACE_W;
      break;
    case MINUTE_:
      return X_SPACE + 4 * CHAR_SPACE_W;
      break;
    default:
      return X_SPACE + CHAR_SPACE_W + (static_cast<int>(a) - 5)  * 2 * CHAR_SPACE_W;
      break;
  }
}

short getY(AlarmEdit a) {
  switch (a) {
    case ID:
    case SET:
      return TIMER + ROW_SPACE;
      break;
    case HOUR_:
    case MINUTE_:
      return TIMER + 2 * ROW_SPACE;
      break;
    default:
      return TIMER + 3 * ROW_SPACE;
      break;
  }
}

short getWidth(AlarmEdit a) {
  switch (a) {
    case SET:
      return 3;
      break;
    case HOUR_:
    case MINUTE_:
      return 2;
      break;
    default:
      return 1;
      break;
  }
}

Clock::Clock(Kernel* kernel) : App(kernel) {}

void Clock::run_code(double x, double y, bool special, Kernel* kernel) {
  update_selection(x, y, kernel);
  checkSpecial(x, y, special, kernel);

  kernel->display.fillRoundRect(0, current_selection, 7, 7, 1, WHITE);

  kernel->display.setCursor(0, 0);
  kernel->display.print("Clock");

  kernel->display.drawRGBBitmap(10, Selection::STOPWATCH, clock_bmp, 15, 8);
  kernel->display.setCursor(40, Selection::STOPWATCH);
  tmElements_t sw = kernel->_clock->getStopwatchTime();
  print_time(sw, &kernel->display);
  if (kernel->_clock->secondChanged() and kernel->_clock->stopwatchIsRunning())
    _clearStopwatchArea(kernel);

  kernel->display.drawRGBBitmap(10, Selection::TIMER, timer, 15, 8);
  kernel->display.setCursor(40, Selection::TIMER);
  tmElements_t timer = kernel->_clock->getTimerTime();
  print_time(timer, &kernel->display);
  if (kernel->_clock->secondChanged() and kernel->_clock->timerRunning())
    _clearTimerArea(kernel);

  AlarmStruct* a = kernel->_clock->getAlarm(alarm_number);
  kernel->display.setCursor(10, Selection::ALARMS);
  kernel->display.print(String("Alarm ") + (alarm_number + 1) + ":  ");
  kernel->display.print((a->Set) ? "Set" : "Off");

  kernel->display.setCursor(16, Selection::ALARMS + 10);
  print2(a->Hour, &kernel->display);
  kernel->display.print(":");
  print2(a->Minute, &kernel->display);

  kernel->display.setCursor(16, Selection::ALARMS + 20);
  kernel->display.print("S M T W T F S");
  for (int i =  SUNDAY; i < NOT; i++) {
    int wday = i - 4;
    AlarmEdit ae = static_cast<AlarmEdit>(i);
    if (a->Days.find(wday) != a->Days.end()) {
      _underline(getX(ae), getY(ae) + 1, getWidth(ae), kernel, false);
    }
  }
}

void Clock::update_selection(double x, double y, Kernel* kernel) {
  if (edit == TimeSelection::NOT_EDITING and alarm_edit == NOT and not (abs(x) > 0.9 and (current_selection == Selection::TIMER or current_selection == ALARMS))) {
    just_moved_x = false;
    if (abs(y) > 0.6 && !just_moved_y) {
      (y > 0) ? ++current_selection : --current_selection;
      just_moved_y = true;

      kernel->display.fillRect(0, 10, 7, kernel->display.height(), BLACK);
    } else if (abs(y) < 0.2) {
      just_moved_y = false;
    }
  } else if ((edit != TimeSelection::NOT_EDITING or (abs(x) > 0.9 and current_selection == Selection::TIMER)) and !kernel->_clock->timerRunning()) {
    if (abs(y) > 0.9 && !just_moved_y) {
      Add dir = (y < 0) ? Add::UP : Add::DOWN;
      switch (edit) {
        case HOUR:
          kernel->_clock->timerHour(dir);
          break;
        case MINUTE:
          kernel->_clock->timerMinute(dir);
          break;
        case SECOND:
          kernel->_clock->timerSecond(dir);
          break;
      }
      clear_chars(edit, current_selection, 2, &kernel->display);
      just_moved_y = true;
    } else if (abs(y) < 0.2) {
      just_moved_y = false;
    }

    if (abs(x) > 0.9 && !just_moved_x) {
      (x > 0) ? ++edit : --edit;
      if (edit != NOT_EDITING) _underline(edit, current_selection, 2, kernel);
      just_moved_x = true;
    } else if (abs(x) < 0.2) {
      just_moved_x = false;
    }
  } else if (alarm_edit != NOT or (abs(x) > 0.9 and current_selection == Selection::ALARMS)) {
    if (abs(y) > 0.6 && !just_moved_y) {
      short dir = -1 * abs(y) / y; //Either 1 or -1, -1 because joystick is inverted
      bool up = (abs(y) / y + 1) / 2; //Either 1 or 0
      AlarmStruct* a = kernel->_clock->getAlarm(alarm_number);
      switch (alarm_edit) {
        case ID:
          if(alarm_number == 0 and dir == -1) {
            alarm_number = NUM_ALARMS - 1;
          } else {
            alarm_number += dir;
            alarm_number %= NUM_ALARMS;
          }
          
          _clearAlarmArea(kernel);
          break;
        case SET:
          a->Set = !a->Set;
          clear_chars(getX(SET), getY(SET), getWidth(SET), &kernel->display);
          break;
        case HOUR_:
          if (a->Hour == 0 and dir == -1) {
            a->Hour = 23;
          } else {
            a->Hour += dir;
            a->Hour %= 24;
          }
          clear_chars(getX(HOUR_), getY(HOUR_), getWidth(HOUR_), &kernel->display);
          break;
        case MINUTE_:
          if (a->Minute == 0 and dir == -1) {
            a->Minute = 59;
          } else {
            a->Minute += dir;
            a->Minute %= 60;
          }
          clear_chars(getX(MINUTE_), getY(MINUTE_), getWidth(MINUTE_), &kernel->display);
          break;
        default:
          int wday = static_cast<int>(alarm_edit) - 4;
          if(a->Days.find(wday) != a->Days.end()) {
            a->Days.erase(wday);
          } else {
            a->Days.insert(wday);
          }
          break;
      }
      
      just_moved_y = true;
    } else if (abs(y) < 0.2) {
      just_moved_y = false;
    }

    if (abs(x) > 0.9 && !just_moved_x) {
      (x > 0) ? ++alarm_edit : --alarm_edit;
      _underline(getX(alarm_edit), getY(alarm_edit), getWidth(alarm_edit), kernel);
      just_moved_x = true;
    } else if (abs(x) < 0.2) {
      just_moved_x = false;
    }
  }
}

void Clock::_clearStopwatchArea(Kernel* kernel) {
  kernel->display.fillRect(40, Selection::STOPWATCH, kernel->display.width(), 7, BLACK);
}

void Clock::_clearTimerArea(Kernel* kernel) {
  kernel->display.fillRect(40, Selection::TIMER, kernel->display.width(), 9, BLACK);
}

void Clock::_clearAlarmArea(Kernel* kernel) {
  kernel->display.fillRect(X_SPACE, ALARMS, kernel->display.width(), CHAR_HEIGHT, BLACK);
  kernel->display.fillRect(X_SPACE + CHAR_SPACE_W, ALARMS + ROW_SPACE, kernel->display.width(), CHAR_HEIGHT, BLACK);
  //kernel->display.fillRect(X_SPACE + CHAR_SPACE_W, ALARMS + 2 * ROW_SPACE, kernel->display.width(), CHAR_HEIGHT, BLACK);
  kernel->display.drawFastHLine(0, ALARMS + 2 * ROW_SPACE + CHAR_HEIGHT + 2, kernel->display.width(), BLACK);
}

void Clock::_underline(short x, short y, short num_chars, Kernel* kernel, bool clear) {
  if (clear) {
    kernel->display.drawFastHLine(0, 20 + CHAR_HEIGHT + 1, kernel->display.width(), BLACK);
    kernel->display.drawFastHLine(0, 30 + CHAR_HEIGHT + 1, kernel->display.width(), BLACK);
    kernel->display.drawFastHLine(0, 40 + CHAR_HEIGHT + 1, kernel->display.width(), BLACK);
    kernel->display.drawFastHLine(0, 50 + CHAR_HEIGHT + 1, kernel->display.width(), BLACK);
  }

  kernel->display.drawFastHLine(x, y + CHAR_HEIGHT + 1, num_chars * CHAR_SPACE_W, WHITE);
}

String Clock::get_name() {
  return String("Clock");
}

void Clock::checkSpecial(double x, double y, bool special, Kernel* kernel) {
  switch (current_selection) {
    case Selection::STOPWATCH:
      if (special and not kernel->getLastSpecial()) {
        if (abs(x) > 0.7) {
          kernel->_clock->clearStopwatch();
          _clearStopwatchArea(kernel);
        } else {
          kernel->_clock->toggleStopwatch();
        }
      }
      break;
    case TIMER:
      if (special and not kernel->getLastSpecial()) {
        if (edit == NOT_EDITING) {
          if (abs(x) > 0.7) {
            kernel->_clock->clearTimer();
            _clearTimerArea(kernel);
          } else {
            kernel->_clock->toggleTimer();
          }
        } else {
          edit = NOT_EDITING;
          _clearTimerArea(kernel);
        }
      }
      break;
    case ALARMS:
      if (special and not kernel->getLastSpecial()) {
        alarm_edit = NOT;
        kernel->_clock->writeAlarms();
        kernel->display.fillScreen(BLACK);
      }
    default:

      break;
  }
}
