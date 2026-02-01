// This software is licensed under the MIT License.
// See the license file for details.
// For more details visit github.com/spacehuhn/DeauthDetector

// include necessary libraries
#include <ESP8266WiFi.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// include ESP8266 Non-OS SDK functions
extern "C" {
#include "user_interface.h"
}

// ===== SETTINGS ===== //
#define LED 2              /* LED pin (2=built-in LED) */

#define BUZZER D5           /* Buzzer pin */
#define SPEED 1.5          /* Song speed, the bigger the number the slower the song */

#define LED_INVERT true    /* Invert HIGH/LOW for LED */
#define SERIAL_BAUD 115200 /* Baudrate for serial communication */
#define CH_TIME 140        /* Scan time (in ms) per channel */
#define PKT_RATE 5         /* Min. packets before it gets recognized as an attack */
#define PKT_TIME 1         /* Min. interval (CH_TIME*CH_RANGE) before it gets recognized as an attack */


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     LED_BUILTIN // Reset pin # (or -1 if sharing Arduino reset pin)

//#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Channels to scan on (US=1-11, EU=1-13, JAP=1-14)
const short channels[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13/*,14*/ };
const String spin = "-\\|/";


// ===== Runtime variables ===== //
int ch_index { 0 };               // Current index of channel array
int packet_rate { 0 };            // Deauth packet counter (resets with each update)
int attack_counter { 0 };         // Attack counter
unsigned long update_time { 0 };  // Last update time
unsigned long update_time_display { 0 };  // Last update for display
unsigned long ch_time { 0 };      // Last channel hop time

unsigned long total_attack_counter { 0 }; //Total attacks counter

bool ATTACK { false }; //
int cc2 { 0 };                    // Another counter
int cc3 { 0 };                    // Another counter
int packets_count { 0 };          // Last Deauth packets counts

unsigned long song_time { 0 };    // Last song update
bool song_playing { false };      // If a song is currently playing
int note_index { 0 };             // Index of note that is currently playing
int note_time { 0 };              // The amount of time (ms) a note is played 

// ===== Notes ===== //
// Borrowed with <3 from here: https://github.com/xitangg/-Pirates-of-the-Caribbean-Theme-Song
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988

int notes[] {
   NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
   NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
   NOTE_A4, NOTE_G4, NOTE_A4, 0,

   NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
   NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
   NOTE_A4, NOTE_G4, NOTE_A4, 0,

   NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
   NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
   NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
   NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
   NOTE_D5, NOTE_E5, NOTE_A4, 0,
   NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
   NOTE_C5, NOTE_A4, NOTE_B4, 0,

   NOTE_A4, NOTE_A4,
   //Repeat of first part
   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
   NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
   NOTE_A4, NOTE_G4, NOTE_A4, 0,

   NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
   NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
   NOTE_A4, NOTE_G4, NOTE_A4, 0,

   NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
   NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
   NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
   NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
   NOTE_D5, NOTE_E5, NOTE_A4, 0,
   NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
   NOTE_C5, NOTE_A4, NOTE_B4, 0,
   //End of Repeat

   NOTE_E5, 0, 0, NOTE_F5, 0, 0,
   NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
   NOTE_D5, 0, 0, NOTE_C5, 0, 0,
   NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4,

   NOTE_E5, 0, 0, NOTE_F5, 0, 0,
   NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
   NOTE_D5, 0, 0, NOTE_C5, 0, 0,
   NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4
};

int duration[] {
  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 375, 125,

  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 375, 125,

  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 125, 250, 125,

  125, 125, 250, 125, 125,
  250, 125, 250, 125,
  125, 125, 250, 125, 125,
  125, 125, 375, 375,

  250, 125,
  //Rpeat of First Part
  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 375, 125,

  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 375, 125,

  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 250, 125, 125,
  125, 125, 125, 250, 125,

  125, 125, 250, 125, 125,
  250, 125, 250, 125,
  125, 125, 250, 125, 125,
  125, 125, 375, 375,
  //End of Repeat

  250, 125, 375, 250, 125, 375,
  125, 125, 125, 125, 125, 125, 125, 125, 375,
  250, 125, 375, 250, 125, 375,
  125, 125, 125, 125, 125, 500,

  250, 125, 375, 250, 125, 375,
  125, 125, 125, 125, 125, 125, 125, 125, 375,
  250, 125, 375, 250, 125, 375,
  125, 125, 125, 125, 125, 500
};


// ===== Sniffer function ===== //
void sniffer(uint8_t *buf, uint16_t len) {
  if (!buf || len < 28) return; // Drop packets without MAC header

  byte pkt_type = buf[12]; // second half of frame control field
  //byte* addr_a = &buf[16]; // first MAC address
  //byte* addr_b = &buf[22]; // second MAC address

  // If captured packet is a deauthentication or dissassociaten frame
  if (pkt_type == 0xA0 || pkt_type == 0xC0) {
    ++packet_rate;
  }
}

void display_string(String input){
  String msg;
  String msg2;

  short ch = channels[ch_index];

  if (ATTACK != true) {
    if (cc3 <= 10){msg = "(o_o)";}
    else if (10 < cc3 and cc3 <= 20){msg = "(O_o)";}
    else if (20 < cc3 and cc3 <= 30){msg = "(O_O)";}
    else if (30 < cc3 and cc3 <= 40){msg = "(o_O)";}
  }
  else{
    msg = "(^v^)";
  }

  msg = msg + "    " + String(spin[cc2]) + "\n";
  msg = msg + input + "\n";
  msg = msg + "Pks :" + String(packets_count)+"\n";
  msg = msg + "Pk/s:" + String(packet_rate)+"\n";
  msg2 = "A:" + String(total_attack_counter) + "\n      C:" + String(ch);
  display.setCursor(0,0);
  display.println(msg);
  display.setTextSize(2);
  display.setCursor(68,0);
  display.println(msg2);
  display.setTextSize(1);
  display.display();
  display.clearDisplay();
}

// ===== Attack detection functions ===== //
void attack_started() {
  song_playing = true;
  note_index = 0;
  note_time = duration[note_index] * SPEED;
  total_attack_counter++;

  digitalWrite(LED, !LED_INVERT); // turn LED on
  ATTACK = true;
  packets_count = 0;
  Serial.println("ATTACK DETECTED");
}

void attack_stopped() {
  song_playing = false;
  noTone(BUZZER); // Stop playing

  digitalWrite(LED, LED_INVERT); // turn LED off
  ATTACK = false;
  Serial.println("ATTACK STOPPED");
}

// ===== Setup ===== //
void setup() {
  Serial.begin(SERIAL_BAUD); // Start serial communication

  pinMode(LED, OUTPUT); // Enable LED pin
  digitalWrite(LED, LED_INVERT);

  pinMode(BUZZER, OUTPUT); // Init buzzer pin

  // Wait for display
  delay(500);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  WiFi.disconnect();                   // Disconnect from any saved or active WiFi connections
  wifi_set_opmode(STATION_MODE);       // Set device to client/station mode
  wifi_set_promiscuous_rx_cb(sniffer); // Set sniffer function
  wifi_set_channel(channels[0]);        // Set channel
  wifi_promiscuous_enable(true);       // Enable sniffer

  display.display();
  delay(1000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  Serial.println("Started \\o/");
}

// ===== Loop ===== //
void loop() {
  unsigned long current_time = millis(); // Get current time (in ms)

  // Update each second (or scan-time-per-channel * channel-range)
  if (current_time - update_time >= (sizeof(channels)*CH_TIME)) {
    update_time = current_time; // Update time variable

    // When detected deauth packets exceed the minimum allowed number
    if (packet_rate >= PKT_RATE) {
      ++attack_counter; // Increment attack counter
    } else {
      if(attack_counter >= PKT_TIME) attack_stopped();
      attack_counter = 0; // Reset attack counter
    }

    // When attack exceeds minimum allowed time
    if (attack_counter == PKT_TIME) {
      attack_started();
    }

//    if (packet_rate != 0) {
//      packets_count = int(packet_rate);
//    }

    Serial.print("Packets/s: ");
    Serial.print(packet_rate);
    Serial.print(", Attacks: ");
    Serial.println(total_attack_counter);

    packet_rate = 0; // Reset packet rate
  }


  if (current_time - update_time_display >= 500) {
    update_time_display = current_time; // Update time variable
    if (ATTACK == true){
          packets_count += int(packet_rate);
          display_string("!Attack!");
    }
    else{
          display_string("__Scan__");
    }
    // counters for display stuff
    cc2 += 1;
    if (cc2 == 4) { cc2 = 0;}
    cc3 += 1;
    if (cc3 == 41) { cc3 = 0;}

  }
  // Channel hopping
  if (sizeof(channels) > 1 && current_time - ch_time >= CH_TIME) {
    ch_time = current_time; // Update time variable

    // Get next channel
    ch_index = (ch_index+1) % (sizeof(channels)/sizeof(channels[0]));
    short ch = channels[ch_index];

    // Set channel
    //Serial.print("Set channel to ");
    //Serial.println(ch);
    wifi_set_channel(ch);
  }

  if(song_playing && current_time - song_time >= note_time) {
    song_time = current_time;

    note_index = (note_index+1) % (sizeof(notes)/sizeof(notes[0]));
    note_time = duration[note_index] * SPEED;

    tone(BUZZER, notes[note_index], note_time);
  }
}