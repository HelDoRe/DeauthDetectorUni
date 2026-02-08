#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#define ENABLE_GxEPD2_GFX 0
// ESP8266 CS(SS)=15,SCL(SCK)=14,SDA(MOSI)=13,BUSY=16,RES(RST)=5,DC=4
#define CS_PIN (15)
#define BUSY_PIN (16)
#define RES_PIN (5)
#define DC_PIN (4)

#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS GxEPD2_154_D67
#define SCREEN_WIDTH 200  // OLED display width, in pixels
#define SCREEN_HEIGHT 200 // OLED display height, in pixels
// 1.54'' EPD Module
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT> display(GxEPD2_DRIVER_CLASS(/*CS=*/CS_PIN, /*DC=*/DC_PIN, /*RES=*/RES_PIN, /*BUSY=*/BUSY_PIN));
