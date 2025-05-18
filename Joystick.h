#pragma once

void setup_joystick(int xpin, int ypin, int special_pin);

int read_joystick_axis(int axis_pin, int analog_pin);
double conform_joystick_axis(int axis_pin, int analog_pin, int minv, int cv, int maxv);
