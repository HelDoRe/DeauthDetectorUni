#include <Arduino.h>
#include "conf.h"
#include "display/mini_tv_144.h"
#include "language.h"

#if DISPLAY_TYPE == 3

short tbx, tby;
unsigned short tbw, tbh;
int x, y;

String old_msg, old_input;
int old_packet_rate, old_packets_count, old_hour, old_minute, old_day, old_month, old_year;
unsigned long old_total_attack_counter;


//#ifndef D_INVERT_COLORS
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000
#define ALERT_COLOR 0xF800
#define GREEN_COLOR 0x07E0
#define GREY_COLOR 0xA618
//#else
//#define FG_COLOR 0x0000
//#define BG_COLOR 0xFFFF
//#endif


SPIClass spi(SPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);

void display_init()
{
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);

    spi.begin(SCK_PIN, -1, MOSI_PIN, -1);

    tft.initR(INITR_144GREENTAB);
    tft.setRotation(D_ROTATION);
#ifndef D_INVERT_COLORS
     tft.invertDisplay(0);
#else
     tft.invertDisplay(1);
#endif
    tft.fillScreen(BG_COLOR);
    tft.setTextColor(GREY_COLOR, BG_COLOR);

          tft.drawFastHLine(0,0,128,GREY_COLOR);
          tft.drawFastHLine(0,18,128,GREY_COLOR);
          tft.drawFastHLine(0,106,128,GREY_COLOR);
          // tft.drawFastVLine(80,0,18,GREY_COLOR);
          tft.drawFastHLine(0,88,128,GREY_COLOR);
          // tft.drawFastVLine(32,88,18,GREY_COLOR);
          tft.drawFastVLine(78,88,18,GREY_COLOR);
          tft.drawFastVLine(40,106,20,GREY_COLOR);
          tft.drawFastHLine(0,127,128,GREY_COLOR);

    display_update_interval = 500; // Set display update interval to 500 ms
}
void display_first_time(String input)
{
}
void display_update(String input, String msg, char spin, int packet_rate, int packets_count, unsigned long total_attack_counter)
{
  char timeBuffer[32];

    time(&now);
    localtime_r(&now, &local_tm);
    curHour = local_tm.tm_hour;
    curMinute = local_tm.tm_min;
    curDay = local_tm.tm_mday;
    curMonth = local_tm.tm_mon;
    curYear = local_tm.tm_year;

  tft.setTextColor(GREY_COLOR, BG_COLOR);
  tft.setCursor(120, 6);
  tft.print(spin);


    if (input != old_input)
    {
        tft.setFont();
        tft.getTextBounds(old_input.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((tft.width() - tbw) / 2) - tbx;
        tft.fillRect(x - 1, 70, tbw + 2, tbh , BG_COLOR);
            tft.getTextBounds(input.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
            x = ((tft.width() - tbw) / 2) - tbx;
            tft.setCursor(x, 70);
            if (!ATTACK)
            {
                tft.setTextColor(GREEN_COLOR, BG_COLOR);
            }
            else
            {
                tft.setTextColor(ALERT_COLOR, BG_COLOR);
            }
            tft.print(input);
    }

      if (msg != old_msg)
    {
      tft.setFont();
      tft.getTextBounds(msg.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
      x = ((tft.width() - tbw) / 2) - tbx;
      tft.setCursor(x, 6);
            if (!ATTACK)
            {
                tft.setTextColor(GREEN_COLOR, BG_COLOR);
            }
            else
            {
                tft.setTextColor(ALERT_COLOR, BG_COLOR);
            }
      tft.print(msg);
    }

   if (curHour != old_hour || curMinute != old_minute)
    {
        tft.setFont(&BASE_FONT);
        sprintf(timeBuffer, "%02d:%02d", old_hour, old_minute);
        tft.getTextBounds((const char *)timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((tft.width() - tbw) / 2) - tbx;
        tft.fillRect(x - 1, 40 - tbh, tbw + 2, tbh + 1, BG_COLOR);
        sprintf(timeBuffer, "%02d:%02d", curHour, curMinute);
        tft.getTextBounds((const char *)timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((tft.width() - tbw) / 2) - tbx;
        tft.setCursor(x, 40);
        tft.setTextColor(FG_COLOR, BG_COLOR);
        tft.print(timeBuffer);
    }
    if (curDay != old_day || curMonth != old_month || curYear != old_year)
    {
        tft.setFont();
        tft.setTextColor(GREY_COLOR, BG_COLOR);
        sprintf(timeBuffer, "%04d/%02d/%02d", 1900 + old_year, 1 + old_month, old_day);
        tft.getTextBounds((const char *)timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((tft.width() - tbw) / 2) - tbx;
        tft.fillRect(x, 40 + 4, tbw + 4, tbh + 2, BG_COLOR);
        sprintf(timeBuffer, "%04d/%02d/%02d", 1900 + curYear, 1 + curMonth, curDay);
        tft.getTextBounds((const char *)timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((tft.width() - tbw) / 2) - tbx;
        tft.setCursor(x, 40 + tbh + 3);
        tft.print(timeBuffer);
    }
    
    tft.setFont();


    old_msg = msg;
    old_input = input;
    old_total_attack_counter = total_attack_counter;
    old_packets_count = packets_count;
    old_packet_rate = packet_rate;
    old_hour = curHour;
    old_minute = curMinute;
    old_day = curDay;
    old_month = curMonth;
    old_year = curYear;
}
#endif