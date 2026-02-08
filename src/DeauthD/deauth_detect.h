#ifndef DEAUTH_DETECT_H
#define DEAUTH_DETECT_H

#include <Arduino.h>

extern int packet_rate;                    // Deauth packet counter (resets with each update)
extern unsigned long total_attack_counter; // Total attacks counter
extern bool ATTACK;                        // is attack currently happening
extern int packets_count;                 // Last Deauth packets counts

void sniffer(uint8_t *buf, uint16_t len);
void attack_started();
void attack_stopped();

#endif
