#include "DeauthD/deauth_detect.h"
#include "conf.h"
#include "language.h"


// ===== Sniffer function ===== //
#ifdef PLATFORM_8266
void sniffer(uint8_t *buf, uint16_t len)
{
    if (!buf || len < 28)
        return; // Drop packets without MAC header

    byte pkt_type = buf[12]; // second half of frame control field
    // byte* addr_a = &buf[16]; // first MAC address
    // byte* addr_b = &buf[22]; // second MAC address

    // If captured packet is a deauthentication or dissassociaten frame
    if (pkt_type == 0xA0 || pkt_type == 0xC0)
    {
        ++packet_rate;
    }
}
#endif
#ifdef PLATFORM_ESP32
void sniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

  if (type == WIFI_PKT_MGMT && (pkt->payload[0] == 0xA0 || pkt->payload[0] == 0xC0 )) ++packet_rate;
    return;
}
#endif

// ===== Attack detection functions ===== //
void attack_started()
{
    total_attack_counter++;
#ifdef LED
    digitalWrite(LED, !LED_INVERT); // turn LED on
#endif
#ifdef LED_E
    digitalWrite(LED_E, !LED_E_INVERT); // turn LED on
#endif
    ATTACK = true;
#ifdef DEBUG_SERIAL
    Serial.println(attack_lng);
#endif
}

void attack_stopped()
{
#ifdef BUZZER
    noTone(BUZZER); // Stop playing
#endif

#ifdef LED
    digitalWrite(LED, LED_INVERT); // turn LED off
#endif
#ifdef LED_E
    digitalWrite(LED_E, LED_E_INVERT); // turn LED off
#endif
    ATTACK = false;
#ifdef DEBUG_SERIAL
    Serial.println(scanning_lng);
#endif
}
