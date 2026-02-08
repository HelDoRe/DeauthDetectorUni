// This software is licensed under the MIT License.
// See the license file for details.
// For more details visit github.com/HelDoRe/DeauthDetectorUni

#include "includes.h"

// include ESP8266 Non-OS SDK functions
/*extern "C" {
#include "user_interface.h"
}*/

#ifdef SYNC_NTP
WiFiManager wifiManager;
#endif

void display_string(String input)
{
  String msg;

  if (ATTACK != true)
  {
    if (cc3 <= 10)
    {
      msg = faces[1];
    }
    else if (10 < cc3 and cc3 <= 20)
    {
      msg = faces[2];
    }
    else if (20 < cc3 and cc3 <= 30)
    {
      msg = faces[3];
    }
    else if (30 < cc3 and cc3 <= 40)
    {
      msg = faces[4];
    }
  }
  else
  {
    msg = faces[0];
  }
// String(spin[cc2])
#ifdef DEBUG_SERIAL
  Serial.println("DEBUG: spin:" + spin[cc2] + ", msg:" + msg + ", display_update_interval: " + String(display_update_interval) + ", song_playing: " + String(song_playing) + ", ATTACK: " + String(ATTACK) + ", packet_rate: " + String(packet_rate) + ", packets_count: " + String(packets_count) + ", total_attack_counter: " + String(total_attack_counter));
#endif

  display_update(input, msg, packet_rate, packets_count, total_attack_counter);
}

// ===== Setup ===== //
void setup()
{
  String msgt;

#ifdef DEBUG_SERIAL
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
  display_init();
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

  WiFi.disconnect();                   // Disconnect from any saved or active WiFi connections
  wifi_set_opmode(STATION_MODE);       // Set device to client/station mode
  wifi_set_promiscuous_rx_cb(sniffer); // Set sniffer function
  wifi_set_channel(channels[0]);       // Set channel
  wifi_promiscuous_enable(true);       // Enable sniffer

  time(&now);
  localtime_r(&now, &local_tm);

#ifdef USE_DISPLAY
  display_first_time(faces[1]);
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
      {
        attack_stopped();
        attack_counter = 0; // Reset attack counter
#ifdef BUZZER
        song_playing = false;
#endif
      }
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

  if (current_time - update_time_display >= display_update_interval)
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