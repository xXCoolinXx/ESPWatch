#include "TouchHandler.hpp"
#include <algorithm>

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

    double rads = std::atan2((double)v.y, (double)v.x);
    if(rads < 0) {
      rads += 2 * M_PI;  
    }

    if(swipe_helper(rads, M_PI/2)) {
      return Gesture::SWIPE_UP;
    } else if(swipe_helper(rads, M_PI)) {
      return Gesture::SWIPE_LEFT;
    } else if(swipe_helper(rads, 3 * M_PI / 2)) {
      return Gesture::SWIPE_DOWN;
    } else if(swipe_helper(rads, 0)) {
      return Gesture::SWIPE_RIGHT;
    }

    return std::nullopt;
  }

  std::tuple<oGesture, oPointInt> Handler::getTouchData() {
    oGesture gesture;
    oPointInt current_point;

    if(!this->_touch.available()) {
      return std::make_tuple(gesture, current_point);
    }
    
    data_struct tp = std::make_tuple(gesture, current_point);
    PointInt pt = PointInt{tp.x, tp.y};

    if(tp.event == Event::DOWN){
      this->resetPoints();
      this->setPoints(pt); 
    } else if (tp.event == Event::CONTACT) {
      if(!initial_point) {
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
      if(!gesture) { gesture = swipeHandler(
            this->initial_point.value(), 
            this->current_point.value()
          ); 
      }

      // Reset points
      this->resetPoints();
    }
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


  void Handler::init() {
    this->_touch.begin();
  }
}
