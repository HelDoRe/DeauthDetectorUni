// include necessary libraries
// #include <Arduino.h>
#include "conf.h"
#include <ESP8266WiFi.h>
#include <time.h>
#ifdef SYNC_NTP
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#endif

#include <Fonts/FreeMonoBold9pt7b.h>
#include "fonts/Outfit_80036pt7b.h"
#include "fonts/Outfit_60011pt7b.h"
#ifdef BUZZER
#include "song.h"
#endif
#include "language.h"
#ifdef DISPLAY
#include "display.h"
#endif
#include "DeauthD/deauth_detect.h"
