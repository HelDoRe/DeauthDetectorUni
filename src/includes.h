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

// ===== Runtime variables ===== //
int ch_index{0};                       // Current index of channel array
int packet_rate{0};                    // Deauth packet counter (resets with each update)
unsigned long attack_counter{0};       // Attack counter
unsigned long update_time{0};          // Last update time
unsigned long update_time_display{0};  // Last update for display
unsigned long ch_time{0};              // Last channel hop time
unsigned long total_attack_counter{0}; // Total attacks counter
bool ATTACK{false};                    // is attack currently happening
int cc2{0};                            // Another counter
int cc3{0};                            // Another counter
int packets_count{0};                  // Last Deauth packets counts

#ifdef LED_E
bool led_ext_blink{false};
#endif

time_t now;
tm local_tm;
int curHour = 0, curMinute = 0, curDay = 0, curMonth = 0, curYear = 0;
