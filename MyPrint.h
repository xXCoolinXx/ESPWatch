#pragma once
#include <TFT_eSPI.h>
#include <SPI.h>
// #include <TimeLib.h>
#include "time.h"

#define CHAR_WIDTH 5
#define CHAR_SPACE_W 6
#define CHAR_HEIGHT 7

void print2(short number, TFT_eSPI* display);

void print2_space(short number, TFT_eSPI* display);

void print_time(struct tm current_time, TFT_eSPI* display, int x, int y);

void clear_chars(short x, short y, short num_chars, TFT_eSPI* display);

String format0(short number);
