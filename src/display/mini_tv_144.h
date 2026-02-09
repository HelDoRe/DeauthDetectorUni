#ifndef mini_TV_144_H
#define mini_TV_144_H

#if DISPLAY_TYPE == 3

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "fonts/Outfit_80036pt7b.h"
#include "fonts/Outfit_60011pt7b.h"
#include "fonts/Font3x7FixedNum.h"

#define BASE_FONT Outfit_60011pt7b   /* Primary font */
#define SMALL_FONT FreeMonoBold9pt7b /* Small info font */
#define BIG_FONT Font3x7FixedNum     /* Big clock font */

#define SD_CS 5
#define TFT_CS 2
#define TFT_DC 0
#define TFT_RST 5
#define MOSI_PIN 4
#define SCK_PIN 3

#define BUTTON_UP 8
#define BUTTON_DOWN 10

static const uint16_t SCREEN_WIDTH = 128;
static const uint16_t SCREEN_HEIGHT = 128;

extern unsigned long display_update_interval;
extern time_t now;
extern tm local_tm;
extern int curHour, curMinute, curDay, curMonth, curYear;
extern bool ATTACK;

void display_init();
void display_first_time(String input);
void display_update(String input, String msg, char spin, int packet_rate, int packets_count, unsigned long total_attack_counter);
#endif
#endif