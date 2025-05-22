#pragma once

const unsigned short screen_width = 240;
const unsigned short screen_height = 240;
const int r = screen_width / 2;
const int r_major = r - 15;
const int r_minor = r - 5;
const int r_cursor = r-20;
const int cursor_w = 5;
const int frame_count = 10;
const int viewbox_wh = 125;

const unsigned short center_x = screen_width / 2;
const unsigned short center_y = screen_height / 2;
const unsigned short left_vb = (screen_width - viewbox_wh) / 2;
const unsigned short right_vb = (screen_width + viewbox_wh) / 2;
const unsigned short top_vb = (screen_height - viewbox_wh) / 2;
const unsigned short bottom_vb = (screen_height + viewbox_wh) / 2;