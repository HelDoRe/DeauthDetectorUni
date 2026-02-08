#ifndef E_PAPER_154_H
#define E_PAPER_154_H

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "fonts/Outfit_80036pt7b.h"
#include "fonts/Outfit_60011pt7b.h"

#define ENABLE_GxEPD2_GFX 0
#define BASE_FONT Outfit_60011pt7b   /* Primary font */
#define SMALL_FONT FreeMonoBold9pt7b /* Small info font */
#define BIG_FONT Outfit_80036pt7b    /* Big clock font */

// ESP8266 CS(SS)=15,SCL(SCK)=14,SDA(MOSI)=13,BUSY=16,RES(RST)=5,DC=4
#define CS_PIN (15)
#define BUSY_PIN (16)
#define RES_PIN (5)
#define DC_PIN (4)
#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS GxEPD2_154_D67
#define SCREEN_WIDTH 200  // OLED display width, in pixels
#define SCREEN_HEIGHT 200 // OLED display height, in pixels
extern unsigned long display_update_interval;
extern time_t now;
extern tm local_tm;
extern int curHour, curMinute, curDay, curMonth, curYear;

void display_init();
void display_first_time(String input);
void display_update(String input, String msg, int packet_rate, int packets_count, unsigned long total_attack_counter);

#endif