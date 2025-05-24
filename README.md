# ESPWatch
ESPWatch for the Arduino <br>

This is a project I made back in 2017. I had the initial goal of creating a fully functioning OS to put on your watch where you could play games and stuff, but I was never able to finish it. I did pick up the project again 2022 and finally in 2025. I am using an ESP32 with a joystick, GAC901 screen, a DS1307 clock, a joystick, a button and a speaker. 

The watch features an analog clock, a digital clock, a modular app system, a Pong app, a Snake app and a News app.

# Libraries

The libraries required are TFT_eSPI, ArduinoJson, DS1307RTC, TimeLib, LittleFS, 
# Secrets
The project requires a `Secrets.h` file for WiFi and APIs. It is formatted as follows

```
#pragma once
#define SSID 
#define PASSWORD 
#define GUARDIAN_API "&api-key=TOKEN_HERE"
#define HF_TOKEN
```
