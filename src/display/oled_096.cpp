#include <Arduino.h>
#include "conf.h"
#include "display/oled_096.h"
#include "language.h"

#if DISPLAY_TYPE == 2
short tbx, tby;
unsigned short tbw, tbh;
int x, y;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void display_init() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        #ifdef DEBUG_SERIAL
        Serial.println(F("SSD1306 allocation failed"));
        #endif
     }
  display.display();
  display.clearDisplay();  
  display.setRotation(D_ROTATION);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.getTextBounds(Title, 0, 0, &tbx, &tby, &tbw, &tbh);
  x = ((display.width() - tbw) / 2) - tbx;
  y = 0;
        display.setCursor(x, y);
        display.print(Title);

        display.getTextBounds(VersionLong, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
  y = y + tbh + 4;
        display.setCursor(x, y);
        display.print(VersionLong);

#ifdef SYNC_NTP
        display.getTextBounds(String(findAP_lng) + "\"" + String(AP_NAME) + "\"", 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
  y = y + tbh + 4;
        display.setCursor(x, y);
        display.print(String(findAP_lng) + "\"" + String(AP_NAME) + "\"");
#endif
display.display();
display_update_interval = 500; // Set display update interval to 500 ms

}
void display_first_time(String input) {
  display.clearDisplay();  

        display.setCursor(1, 0);
        display.print(input);
        display.setCursor(1, 8);
        display.print(pkt_lng + "0");
        display.setCursor(1, 16);
        display.print(packs_lng + "0");
        display.setCursor(1, 24);
        display.print(attacks_lng + "0");
  display.display();
}
void display_update(String input, String msg, char spin, int packet_rate, int packets_count, unsigned long total_attack_counter) {
  display.clearDisplay();  

        display.setCursor(1, 0);
        display.print(msg);
        display.setCursor((display.width() / 2) - 16, 0);
        display.print(input);
        display.setCursor(1, 8);
        display.print(pkt_lng + String(packets_count));
        display.setCursor(1, 16);
        display.print(packs_lng + String(packet_rate));
        display.setCursor(1, 24);
        display.print(attacks_lng + String(total_attack_counter));
          display.setTextSize(2);
        display.setCursor(display.width() - 16, 8);
        display.print(spin);
          display.setTextSize(1);
  display.display();
    
}


#endif