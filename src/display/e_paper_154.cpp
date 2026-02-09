#include <Arduino.h>
#include "conf.h"
#include "display/e_paper_154.h"
#include "language.h"

#if DISPLAY_TYPE == 1
short tbx, tby;
unsigned short tbw, tbh;
int x, y;

String old_msg, old_input;
int old_packet_rate, old_packets_count, old_hour, old_minute, old_day, old_month, old_year;
unsigned long old_total_attack_counter;

// 1.54" EPD Module
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT> display(GxEPD2_DRIVER_CLASS(/*CS=*/CS_PIN, /*DC=*/DC_PIN, /*RES=*/RES_PIN, /*BUSY=*/BUSY_PIN));

#ifndef D_INVERT_COLORS
#define FG_COLOR GxEPD_BLACK
#define BG_COLOR GxEPD_WHITE
#else
#define FG_COLOR GxEPD_WHITE
#define BG_COLOR GxEPD_BLACK
#endif

void display_init()
{
    // ePaper
    display.init(SERIAL_BAUD, true, 10, false);
    display.setRotation(D_ROTATION);
    display.setFont(&BASE_FONT);
    display.setTextColor(FG_COLOR);

    display.getTextBounds(Title, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center the bounding box by transposition of the origin:
    x = ((display.width() - tbw) / 2) - tbx;
    y = ((display.height() - tbh) / 2) - tby;
    display.setFullWindow();
    display.firstPage();
    do
    {
        display.fillScreen(BG_COLOR);
        display.setCursor(x, y - tbh);
        display.print(Title);

        display.getTextBounds(VersionLong, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, y + tbh);
        display.print(VersionLong);

#ifdef SYNC_NTP
        display.setTextColor(display.epd2.hasColor ? GxEPD_RED : FG_COLOR);
        display.getTextBounds(String(findAP_lng) + "\"" + String(AP_NAME) + "\"", 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, y + (tbh * 3));
        display.print(String(findAP_lng) + "\"" + String(AP_NAME) + "\"");
#endif
    } while (display.nextPage());

    display_update_interval = 4000; // Set display update interval to 4 second (4000 ms)
}

void display_first_time(String input)
{
    display.setFont(&SMALL_FONT);
    display.setPartialWindow(0, 0, 200, 200);
    display.fillScreen(BG_COLOR);
    display.setTextColor(FG_COLOR);
    display.firstPage();
    do
    {
        display.setCursor(6, 165);
        display.print(pkt_lng + "0");
        display.setCursor(6, 180);
        display.print(packs_lng + "0");
        display.setCursor(6, 195);
        display.print(attacks_lng + "0");
        display.setFont(&BASE_FONT);
        display.setTextSize(1);
        display.getTextBounds(scanning_lng.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 53);
        display.print(scanning_lng);
        display.setTextSize(2);
        display.getTextBounds(input.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 30);
        display.print(input.c_str());
    } while (display.nextPage());
    display.setFont(&BASE_FONT);
    display.hibernate();
}

void display_update(String input, String msg, char spin,int packet_rate, int packets_count, unsigned long total_attack_counter)
{
    char timeBuffer[32];

    time(&now);
    localtime_r(&now, &local_tm);
    curHour = local_tm.tm_hour;
    curMinute = local_tm.tm_min;
    curDay = local_tm.tm_mday;
    curMonth = local_tm.tm_mon;
    curYear = local_tm.tm_year;

    if (packet_rate != old_packet_rate || packets_count != old_packets_count || total_attack_counter != old_total_attack_counter)
    {

        display.setFont(&SMALL_FONT);
        display.setPartialWindow(100, 150, 100, 50);
        display.firstPage();
        do
        {
            display.setTextSize(1);
            display.setCursor(106, 165);
            display.print(String(packets_count));
            display.setCursor(106, 180);
            display.print(String(packet_rate));
            display.setCursor(106, 195);
            display.print(String(total_attack_counter));
        } while (display.nextPage());
    }

    if (input != old_input)
    {
        display.setFont(&BASE_FONT);
        display.setPartialWindow(0, 39, 200, 15);
        display.firstPage();
        do
        {
            display.setTextSize(1);
            display.getTextBounds(input.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
            x = ((display.width() - tbw) / 2) - tbx;
            display.setCursor(x, 54);
            display.print(input);
        } while (display.nextPage());
    }

    if (msg != old_msg)
    {
        display.setFont(&BASE_FONT);
        display.setPartialWindow(0, 0, 200, 35);
        display.firstPage();
        do
        {
            display.setTextSize(2);
            display.getTextBounds(msg.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
            x = ((display.width() - tbw) / 2) - tbx;
            display.setCursor(x, 30);
            display.print(msg);
        } while (display.nextPage());
    }

    if (curHour != old_hour || curMinute != old_minute)
    {
        display.setPartialWindow(0, 65, 200, 55);
        display.firstPage();
        display.setFont(&BIG_FONT);
        display.setTextSize(1);
        sprintf(timeBuffer, "%02d:%02d", curHour, curMinute);
        display.getTextBounds((const char *)timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        if (curHour < 1 && curMinute < 10)
         x = 0;
         else
         x = ((display.width() - tbw) / 2) - tbx;
        do
        {
            display.setCursor(x, 115);
            display.print(timeBuffer);
        } while (display.nextPage());
    }
    display.setFont(&BASE_FONT);
    if (curDay != old_day || curMonth != old_month || curYear != old_year)
    {
        display.setPartialWindow(0, 120, 200, 15);
        display.firstPage();
        display.setTextSize(1);
        sprintf(timeBuffer, "%04d/%02d/%02d", 1900 + curYear, 1 + curMonth, curDay);
        display.getTextBounds((const char *)timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        do
        {
            display.setCursor(x, 135);
            display.print(timeBuffer);
        } while (display.nextPage());
    }

    display.hibernate();

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