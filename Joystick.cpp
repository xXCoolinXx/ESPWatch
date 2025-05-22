#include "Joystick.h"
#include <Arduino.h>

double read_joystick_axis(int axis_pin) {
  return analogRead(axis_pin) * 3.3 / 4095; // Read the pin an convert it to a voltage
}

const double cv = 1.45; // Voltage at rest
const double tolerance = 0.2;
//Puts joystick input on scale from -1 to 1 to make the numbers easier to work with
double conform_joystick_axis(int axis_pin) 
{
  double v = read_joystick_axis(axis_pin);

  if(cv - tolerance <= v && v <= cv + tolerance) { return 0.0; } // Pin is at center

  v -= cv; // Center v

  // Calculate percent (times -1 or 1) of way to endpoint, giving values from -1 to 1
  if(v < cv) {
    return max(-1.0, v / (cv - 0.0));
  } else { // Greater than 1.45
    return min(1.0, v / (3.3 - cv));
  }
}
