/*
 * Handles gesture tracking and management for the CST816S
 * Both driver libraries (CST816S, SensorLib->TouchDrv) fail for various reasons with gestures
 * The former at least has gestures, but they are the hardware level gestures, which suck
 * As such, TouchHandler is necessary to do gesture tracking at the software level
 * Thank you for your attention to this matter
*/
#pragma once
#include <Arduino.h>
#include <CST816S.h>
#include <optional>
#include <Shapes.h>
#include <Wire.h>

typedef std::optional<PointInt> oPointInt;

namespace TouchHandler {
  const unsigned long TOUCH_TIMEOUT = 150;
  const double MIN_SWIPE_DISTANCE = 20.0;
  const double MAX_SWIPE_ANGLE_FROM_AXIS = 2 * M_PI / 3; // 30 degrees
  const int MAX_TAP_DISTANCE = 10;
  const unsigned long MAX_TAP_DURATION = 300;
  const unsigned long MIN_LONG_PRESS_DURATION = 1000;

  enum Gesture {
    SWIPE_UP,
    SWIPE_DOWN,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    TAP, 
    LONG_PRESS,
  };

  typedef std::optional<TouchHandler::Gesture> oGesture;

  oGesture swipeHandler(PointInt initial_point, PointInt final_point);
  oPointInt tapHandler(PointInt initial_point, PointInt final_point, unsigned long duration);

  enum Event {
    DOWN = 0,
    UP = 1,
    CONTACT = 2,
  };

  class Handler {
    private:
      oPointInt initial_point; 
      oPointInt current_point;
      unsigned long initial_touch_time = 0; 
      unsigned long last_touch_time = 0; // For handling a missed up signal
    public:
      CST816S _touch = CST816S(TP_SDA, TP_SCL, TP_RST, TP_INT);
      
      // Initialize 
      void init();
       
      void resetPoints();
      void setPoints(PointInt new_point);
      void setCurrentPoint(PointInt new_point);
      
      std::tuple<oGesture, oPointInt> getTouchData();
  };
}
