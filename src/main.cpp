// This software is licensed under the MIT License.
// See the license file for details.
// For more details visit github.com/HelDoRe/DeauthDetectorUni

#include "includes.h"

short tbx, tby;
unsigned short tbw, tbh;
int x, y;

// include ESP8266 Non-OS SDK functions
/*extern "C" {
#include "user_interface.h"
}*/

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

// #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#ifdef SYNC_NTP
WiFiManager wifiManager;
#endif

String old_msg, old_input;
int old_packet_rate, old_packets_count, old_hour, old_minute, old_day, old_month, old_year;
unsigned long old_total_attack_counter;

void display_string(String input)
{
  String msg;

  #ifdef DEBUG_SERIAL
  Serial.println("DEBUG: song_playing: " + String(song_playing) + ", ATTACK: " + String(ATTACK) + ", packet_rate: " + String(packet_rate) + ", packets_count: " + String(packets_count) + ", total_attack_counter: " + String(total_attack_counter));
  #endif
  

  if (ATTACK != true)
  {
    if (cc3 <= 10)
    {
      msg = "(o_o)";
    }
    else if (10 < cc3 and cc3 <= 20)
    {
      msg = "(O_o)";
    }
    else if (20 < cc3 and cc3 <= 30)
    {
      msg = "(O_O)";
    }
    else if (30 < cc3 and cc3 <= 40)
    {
      msg = "(o_O)";
    }
  }
  else
  {
    msg = "(^v^)";
  }

#ifdef USE_DISPLAY
  char timeBuffer[32];
  /*  zrobic refresh tylko wtedy jak cos sie zmienilo. */
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

#endif
  time(&now);
  localtime_r(&now, &local_tm);
  curHour = local_tm.tm_hour;
  curMinute = local_tm.tm_min;
  curDay = local_tm.tm_mday;
  curMonth = local_tm.tm_mon;
  curYear = local_tm.tm_year;

#ifdef USE_DISPLAY
  if (curHour != old_hour || curMinute != old_minute)
  {
    display.setPartialWindow(0, 65, 200, 55);
    display.firstPage();
    display.setFont(&BIG_FONT);
    display.setTextSize(1);
    sprintf(timeBuffer, "%02d:%02d", curHour, curMinute);
    display.getTextBounds((const char *)timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    do
    {
      display.setCursor(x, 115);
      display.print(timeBuffer);
    } while (display.nextPage());
  }
  display.setFont(&BASE_FONT);
  if (curDay != old_day || curMonth != old_month)
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
#endif
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

// ===== Setup ===== //
void setup()
{
  String msgt;

#ifdef SERIAL_DEBUG
  Serial.begin(SERIAL_BAUD); // Start serial communication
#else
  Serial.end(); // End serial communication
#endif

#ifdef SYNC_NTP
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.setDarkMode(true);
  configTime(MY_TZ, MY_NTP_SERVER);
#endif
  // ePaper

#ifdef USE_DISPLAY
  display.init(SERIAL_BAUD, true, 50, false);
  display.setRotation(D_ROTATION);
  display.setFont(&BASE_FONT);
  display.setTextColor(GxEPD_BLACK);

  display.getTextBounds(Title, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  x = ((display.width() - tbw) / 2) - tbx;
  y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y - tbh);
    display.print(Title);

    display.getTextBounds(VersionLong, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    display.setCursor(x, y + tbh);
    display.print(VersionLong);

#ifdef SYNC_NTP
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    display.getTextBounds("Find AP \"" + String(AP_NAME) + "\"", 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    display.setCursor(x, y + (tbh * 3));
    display.print("Find AP \"" + String(AP_NAME) + "\"");
#endif
  } while (display.nextPage());
#endif

#ifdef SYNC_NTP
  wifiManager.autoConnect(AP_NAME);
#endif

#ifdef LED
  pinMode(LED, OUTPUT); // Enable LED pin
  digitalWrite(LED, LED_INVERT);
#endif
#ifdef LED_E
  pinMode(LED_E, OUTPUT); // Enable LED_E pin
  digitalWrite(LED_E, LED_E_INVERT);
#endif

#ifdef BUZZER
  pinMode(BUZZER, OUTPUT); // Init buzzer pin
#endif

#ifdef DEBUG_SERIAL
  Serial.println();
#ifdef USE_DISPLAY
  Serial.println("Display init OK");
#endif
#ifdef SYNC_NTP
  Serial.println("WiFiManager init OK");
#endif
#ifdef LED
  Serial.println("LED init OK");
#endif
#ifdef LED_E
  Serial.println("External LED init OK");
#endif
#ifdef BUZZER
  Serial.println("Buzzer init OK");
#endif
  Serial.println("Init passed, starting...");
#endif

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  /* if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
*/

#ifdef DEBUG_SERIAL
  Serial.print("Waiting for NTP");
#endif
  now = time(nullptr);
  // Loop until time is > 24h since Jan 1 1970 (1st Jan 2010 onwards)
  int break_loop = 0;
  while (now < 24 * 3600 && break_loop < 21)
  {
    delay(500);
#ifdef DEBUG_SERIAL
    Serial.print(".");
#endif
    now = time(nullptr);
    break_loop++;
  }
#ifdef DEBUG_SERIAL
  if (now > 24 * 3600)
    Serial.println(".OK");
  else
    Serial.println("error!");
#endif
  // delay(4000);

  WiFi.disconnect();                   // Disconnect from any saved or active WiFi connections
  wifi_set_opmode(STATION_MODE);       // Set device to client/station mode
  wifi_set_promiscuous_rx_cb(sniffer); // Set sniffer function
  wifi_set_channel(channels[0]);       // Set channel
  wifi_promiscuous_enable(true);       // Enable sniffer

  time(&now);
  localtime_r(&now, &local_tm);

#ifdef USE_DISPLAY
  display.setFont(&SMALL_FONT);

  display.setPartialWindow(0, 0, 200, 200);
  display.firstPage();
  do
  {
    display.setCursor(6, 165);
    display.print("Packets : 0");
    display.setCursor(6, 180);
    display.print("Packs/s : 0");
    display.setCursor(6, 195);
    display.print("Attacks : 0");
    display.setFont(&BASE_FONT);
    display.setTextSize(1);
    display.getTextBounds(scanning_lng.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    display.setCursor(x, 53);
    display.print(scanning_lng);
    display.setTextSize(2);
    display.getTextBounds("(o_o)", 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    display.setCursor(x, 30);
    display.print("(o_o)");
  } while (display.nextPage());
  display.setFont(&BASE_FONT);
  display.hibernate();
#endif

#ifdef DEBUG_SERIAL
  Serial.println("Started \\o/");
#endif
}

// ===== Loop ===== //
void loop()
{
  unsigned long current_time = millis(); // Get current time (in ms)

  // Update each second (or scan-time-per-channel * channel-range)
  if (current_time - update_time >= (sizeof(channels) * CH_TIME))
  {
    update_time = current_time; // Update time variable

    // When detected deauth packets exceed the minimum allowed number
    if (packet_rate >= PKT_RATE)
    {
      ++attack_counter; // Increment attack counter
    }
    else
    {
      if (attack_counter >= PKT_TIME)
        attack_stopped();
      attack_counter = 0; // Reset attack counter
#ifdef BUZZER
    song_playing = false;
#endif
    }

    // When attack exceeds minimum allowed time
    if (attack_counter == PKT_TIME)
    {
#ifdef BUZZER
    // Play the song
    song_playing = true;
    note_index = 0;
    note_time = duration[note_index] * SPEED;
#endif
      attack_started();
    }

#ifdef DEBUG_SERIAL
    Serial.print("Packets/s: ");
    Serial.print(packet_rate);
    Serial.print(", Attacks: ");
    Serial.println(total_attack_counter);
#endif

    packet_rate = 0; // Reset packet rate
  }

  if (current_time - update_time_display >= 3000)
  {
    update_time_display = current_time; // Update time variable
    if (ATTACK == true)
    {
      packets_count += int(packet_rate);
      display_string(attack_lng);
#ifdef LED_E
      led_ext_blink = !led_ext_blink;
      digitalWrite(LED_E, led_ext_blink); // turn LED off
#endif
    }
    else
    {
      display_string(scanning_lng);
    }
    // counters for display stuff
    cc2 += 1;
    if (cc2 == 4)
    {
      cc2 = 0;
    }
    cc3 += 1;
    if (cc3 == 41)
    {
      cc3 = 0;
    }
  }
  // Channel hopping
  if (sizeof(channels) > 1 && current_time - ch_time >= CH_TIME)
  {
    ch_time = current_time; // Update time variable

    // Get next channel
    ch_index = (ch_index + 1) % (sizeof(channels) / sizeof(channels[0]));
    short ch = channels[ch_index];

    // Set channel
    // Serial.print("Set channel to ");
    // Serial.println(ch);
    wifi_set_channel(ch);
  }

#ifdef BUZZER
  if (song_playing && current_time - song_time >= note_time)
  {
    song_time = current_time;

    note_index = (note_index + 1) % (sizeof(notes) / sizeof(notes[0]));
    note_time = duration[note_index] * SPEED;

    tone(BUZZER, notes[note_index], note_time);
  }
#endif
}