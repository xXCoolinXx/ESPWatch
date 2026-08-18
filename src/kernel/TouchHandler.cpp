#include "src/kernel/TouchHandler.hpp"
#include <algorithm>
#include "src/config/GFXConstants.h"

namespace TouchHandler {
  bool swipe_helper(double rads, double axis_rads) {
    return
        rads > axis_rads - MAX_SWIPE_ANGLE_FROM_AXIS && 
        rads < axis_rads + MAX_SWIPE_ANGLE_FROM_AXIS;
  }

  oGesture swipeHandler(PointInt initial_point, PointInt final_point) {
    PointInt v = final_point - initial_point;
    
    if(v.norm() < MIN_SWIPE_DISTANCE) {
      return std::nullopt;
    }  

    double rads = std::atan2(-1.0 * v.y, (double)v.x);
    if(rads < 0) {
      rads += 2 * M_PI;  
    }

    if(swipe_helper(rads, M_PI/2)) {
      return Gesture::SWIPE_UP;
    } else if(swipe_helper(rads, M_PI)) {
      return Gesture::SWIPE_LEFT;
    } else if(swipe_helper(rads, 3 * M_PI / 2)) {
      if(final_point.y > screen_height - DOWN_SLASH_ZONE_WIDTH && initial_point.y < DOWN_SLASH_ZONE_WIDTH) {
        return Gesture::DOWN_SLASH;
      }

      return Gesture::SWIPE_DOWN;
    } else if(swipe_helper(rads, 0) || swipe_helper(rads, 2 * M_PI)) {
      return Gesture::SWIPE_RIGHT;
    }

    return std::nullopt;
  }

  oGesture tapHandler(PointInt initial_point, PointInt final_point, unsigned long duration) {
    PointInt v = final_point - initial_point;
    if(v.norm() < MAX_TAP_DISTANCE) {
      if(duration < MAX_TAP_DURATION) {
        return Gesture::TAP;
      } else if(duration > MIN_LONG_PRESS_DURATION) {
        return Gesture::LONG_PRESS;
      }
    }

    return std::nullopt;
  }

  std::tuple<oGesture, oPointInt> Handler::getTouchData() {
    oGesture gesture;
    oPointInt ret_point = this->current_point; // Always return previous point to avoid issues with clearing 

    if(!this->_touch.available()) {
      return std::make_tuple(gesture, current_point);
    }
    
    data_struct tp = this->_touch.data;
    //     x=240
    //y=0 <- | -> y=240
    //      x=0
    
    // Swap around axes
    PointInt pt = PointInt{tp.y, screen_height - tp.x};

    if(tp.event == Event::DOWN){
      this->resetPoints();
      this->setPoints(pt); 
    } else if (tp.event == Event::CONTACT) {
      if(!initial_point) { //Handle a missed down signal
        this->resetPoints();
        this->setPoints(pt);
      } else {
        this->setCurrentPoint(pt);
      }
    } else if(tp.event == Event::UP || 
      millis() - this->last_touch_time > TOUCH_TIMEOUT) {
      
      if(!this->initial_point || !this->current_point) {
        return std::make_tuple(gesture, this->current_point);
      }

      // Basically just keep going until we have exhausted all gesture processing
      if(!gesture) {
          gesture = swipeHandler(
            this->initial_point.value(), 
            this->current_point.value()
          ); 
      }
      if(!gesture) {
        gesture = tapHandler(
            this->initial_point.value(), 
            this->current_point.value(), 
            this->last_touch_time - this->initial_touch_time
        );
      }

      // Reset points
      this->resetPoints();
    }

    return std::make_tuple(gesture, ret_point);
  }

  void Handler::resetPoints() {
    this->initial_point.reset();
    this->current_point.reset();
    this->initial_touch_time = 0;
    this->last_touch_time = 0;
  }

  void Handler::setPoints(PointInt new_point) {
    this->initial_point = new_point;
    this->current_point = new_point; 
    this->initial_touch_time = millis();
    this->last_touch_time = this->initial_touch_time;
  }

  void Handler::setCurrentPoint(PointInt new_point) {
    this->current_point = new_point;
    this->last_touch_time = millis();
  }

  bool Handler::available() {
    return this->_touch.available(); 
  }

  void Handler::init() {
    this->_touch.begin(); 
 
    // Enable wake from touch
    // gpio_wakeup_enable((gpio_num_t)TP_INT, GPIO_INTR_LOW_LEVEL);
    // esp_sleep_enable_ext0_wakeup((gpio_num_t)TP_INT, 0);
  }
}
