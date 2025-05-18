#include "Joystick.h"
#include <Arduino.h>

void setup_joystick(int xpin, int ypin, int special_pin) {
  pinMode(xpin, OUTPUT);
  pinMode(ypin, OUTPUT);

  pinMode(special_pin, INPUT);
}

int read_joystick_axis(int axis_pin, int analog_pin) {
  digitalWrite(axis_pin, HIGH);
  int value = analogRead(analog_pin);
  digitalWrite(axis_pin, LOW);

  return value;
}

//Puts joystick input on scale from -1 to 1 to make the numbers easier to work with
double conform_joystick_axis(int axis_pin, int analog_pin, int minv, int cv, int maxv) 
{
  int normalized_value = read_joystick_axis(axis_pin, analog_pin) - cv;
  int magnitude = 0;

  if (normalized_value < 0) {
    magnitude = max(-1, -1 * abs(normalized_value / (minv - cv)));
  } else if (normalized_value > 0) {
    magnitude = min(1, abs(normalized_value / (maxv - cv)));
  }

  return magnitude;
}
