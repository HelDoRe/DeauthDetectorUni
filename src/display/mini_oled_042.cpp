#include <Arduino.h>
#include "conf.h"
#include "display/mini_oled_042.h"
#include "language.h"

#if DISPLAY_TYPE == 4

//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 6, 5, U8X8_PIN_NONE); //need offset for 128x64
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, 6, 5, U8X8_PIN_NONE); 

void display_init()
{
  u8g2.begin();
//  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_5x7_tr); // choose a suitable font
//  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.clearBuffer();
//  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 7, Title);
  u8g2.drawStr(15, 15, VersionShort);
  u8g2.drawStr(0, 23, VersionLong);
#ifdef SYNC_NTP
  u8g2.drawStr(0, 31, findAP_lng.c_str());
  u8g2.drawStr(0, 39, AP_NAME);
#endif
  u8g2.sendBuffer();
  display_update_interval = 500; // Set display update interval to 500 ms
// Initialize display here
}
void display_first_time(String input)
{
  u8g2.clearBuffer();
    // Display first time screen here
  u8g2.sendBuffer();delay(2000); // Display first time screen for 2 seconds
}
void display_update(String input, String msg, char spin, int packet_rate, int packets_count, unsigned long total_attack_counter)
{
  char txtBuffer[20];
  u8g2.clearBuffer();
  u8g2.drawStr(0, 7, input.c_str());
  u8g2.drawStr(10, 15, msg.c_str());
  u8g2.drawStr(68, 15, String(spin).c_str());

  sprintf(txtBuffer, "%s%d", pkt_lng, packets_count);
  u8g2.drawStr(0, 23, txtBuffer);
  sprintf(txtBuffer, "%s%d", packs_lng, packet_rate);
  u8g2.drawStr(0, 31, txtBuffer);
  sprintf(txtBuffer, "%s%lu", attacks_lng, total_attack_counter);
  u8g2.drawStr(0, 39, txtBuffer);


    // Update display here
  u8g2.sendBuffer();
}


#endif