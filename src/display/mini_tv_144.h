#ifndef mini_TV_144_H
#define mini_TV_144_H

#if DISPLAY_TYPE == 3
void display_init();
void display_first_time(String input);
void display_update(String input, String msg, int packet_rate, int packets_count, unsigned long total_attack_counter);
#endif
#endif