#include <Arduino.h>
#include "conf.h"
#include "display/mini_tv_144.h"
#include "language.h"

#if DISPLAY_TYPE == 3

void display_init()
{
}
void display_first_time(String input)
{
}
void display_update(String input, String msg, int packet_rate, int packets_count, unsigned long total_attack_counter)
{
}
#endif