# ESPWatch
ESPWatch for the Arduino <br>

This is a project I started back in 2017. I had the initial goal of creating a fully functioning watch system for an Arduino-based device, but I was never able to finish it due to budget constraints and technical limitations. 

I did pick up the project again 2022 and finally in 2025. I am using an ESP32 with a joystick, GAC901 screen (connected via SPI), a DS1307 clock (connected via RTC), a joystick (connected via analog pins), a button and a speaker. 

The watch features an analog clock, a digital clock, a modular app system, a Pong app, a Snake app and a News app. There is also a Clock app that is fully functional but I didn't port it to this latest iteration of the project.

If one were to make this into a physical watch, I would imagine it would be much better to have physical buttons on the side of the watch instead of the joystick. The code is not well suited for interaction with touch, but this would also be possible with a more advanced rewrite.

# Libraries

The libraries required are TFT_eSPI, ArduinoJson, DS1307RTC, TimeLib, and LittleFS.
# Secrets
The project requires a `Secrets.h` file for WiFi and APIs. It is formatted as follows

```
#pragma once
#define SSID 
#define PASSWORD 
#define GUARDIAN_API "&api-key=TOKEN_HERE"
#define HF_TOKEN
```
