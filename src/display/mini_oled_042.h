#ifndef MINI_OLED_042_H
#define MINI_OLED_042_H

#if DISPLAY_TYPE == 4
#include <U8g2lib.h>

static const uint16_t SCREEN_WIDTH = 72;
static const uint16_t SCREEN_HEIGHT = 40;

extern unsigned long display_update_interval;

void display_init();
void display_first_time(String input);
void display_update(String input, String msg, char spin, int packet_rate, int packets_count, unsigned long total_attack_counter);
#endif
#endif