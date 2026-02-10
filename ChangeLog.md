-=# v0.0.6 (20260209) #=- 
ADD: 
 * Super micro ESP32-C3 with 0.42" Oled (remember about WiFi.setTxPower(WIFI_POWER_15dBm) or less, to get WiFi works on ESP32-C3 super mini) 

-=# v0.0.5 (20260209) #=- 
ADD: 
 * Multiplatform in .ini 
 * Start adding support for MiniTV 1.44" 
 * MiniTV show time, date, status.
 * Fix total packets counter
 * Oled_096 support (again ;) 
 * Small fix in display miniTV 

-=# v0.0.4 (20260208) #=- 
ADD: 
 * SERIAL_DEBUG on/off 
 * some cleanup in code

-=# v0.0.3 (20260207) #=- 
ADD: 
 * WiFiManager from https://github.com/tzapu/WiFiManager 
 * LittleFS
 
-=# v0.0.2 (20260201) #=- 
ADD: 
 * platformio.ini
 * display date
 * break loop WiFi and NTP when fail
 * song in external *.h file
 * add another font
 
-=# v0.0.1 (20260131) #=- 
ADD: 
 * font from https://github.com/z88kat/lilygo-paper-watch/
 * NTP time sync from https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm
 * display time
 
-=# v0.0.0 (20260104) #=- 
ADD: 
 * code from https://github.com/spacehuhn/DeauthDetector
 * buzzer support
 * song from https://github.com/xitangg/-Pirates-of-the-Caribbe
 * OLED 0.96" support (based on https://github.com/gdamdam/wifi-deauthdetector)
 * blinking external LED
 * changed OLED to E-PAPER 1.44"