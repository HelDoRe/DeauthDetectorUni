// This software is licensed under the MIT License.
// See the license file for details.
// For more details visit github.com/spacehuhn/DeauthDetector

/* song grac jakos non blocking? jak sie update ePapera robi to bardzo przerywa granie, wiec albo to jakos inaczej, albo sprawdzic jak eInka przyspieszyc.
   rozbic dane, song do .h itp
   porzadki w zmiennych, define, stringi (wyciagnac wyswietlane slowa do zmiennych i omze multilanguage pozniej sie zrobi)
   * dodac zegarek
   dodac date
   dodac config wifi przez web opcjonalnie, jakby czas pociagnac przez ntp
   * data po wifi po uruchomieniu, potem spokoj.
   puscic na githuba
   dodac obsluge przycisku (wolne GPIO D4) i ustawien w locie?
   dodac baterie?
   zapis danych zeby po utracie zasilania nie gubily sie
   przeportowac do platformio
   jakies foty i dokumentacje
   skad inspiracja napisac dac linki
   ToDo i ChangeLog
   dioda power breathe - ze dziala, i plynne, a nie on/off
   define DEBUG ON OFF a potem IFDEF, i jak lib od e-paper ma wyl/wl debuga
 */

const char Title[] = "DeauthDetector";
const char VersionLong[] = "v0.0.1 (20260131)";
const char VersionShort[] = "v0.0.1";
#define D_ROTATION 1
#define DEBUG_LOG 1
int16_t tbx, tby; uint16_t tbw, tbh, x, y;
  #define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
// include necessary libraries
const char ssid[] = "";
const char password[] = "";

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "fonts/Outfit_80036pt7b.h"
#include <ESP8266WiFi.h>
#include <time.h>


// ESP8266 CS(SS)=15,SCL(SCK)=14,SDA(MOSI)=13,BUSY=16,RES(RST)=5,DC=4
#define CS_PIN (15)
#define BUSY_PIN (16)
#define RES_PIN (5)
#define DC_PIN (4)

// 1.54'' EPD Module
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=5*/ CS_PIN, /*DC=*/ DC_PIN, /*RES=*/ RES_PIN, /*BUSY=*/ BUSY_PIN)); // GDEH0154D67 200x200, SSD1681


// include ESP8266 Non-OS SDK functions
extern "C" {
#include "user_interface.h"
}

// ===== SETTINGS ===== //
#define LED 2              /* LED pin (2=built-in LED) */
#define LED_E D3 //D3 to 0 s D4 to GPIO2 //D0 lub 16           /* External LED pin */

#define BUZZER D6 //D7 na nodeMCU          /* Buzzer pin */
#define SPEED 1.5          /* Song speed, the bigger the number the slower the song */

#define LED_INVERT true    /* Invert HIGH/LOW for LED */
#define LED_E_INVERT false    /* Invert HIGH/LOW for LED */
#define SERIAL_BAUD 115200 /* Baudrate for serial communication */
#define CH_TIME 140        /* Scan time (in ms) per channel */
#define PKT_RATE 5         /* Min. packets before it gets recognized as an attack */
#define PKT_TIME 1         /* Min. interval (CH_TIME*CH_RANGE) before it gets recognized as an attack */


#define SCREEN_WIDTH 200 // OLED display width, in pixels
#define SCREEN_HEIGHT 200 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
////#define OLED_RESET     LED_BUILTIN // Reset pin # (or -1 if sharing Arduino reset pin)

//#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
//#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Channels to scan on (US=1-11, EU=1-13, JAP=1-14)
const short channels[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13/*,14*/ };
const String spin = "-\\|/";
const String scanning_lng = "Scanning...";
const String attack_lng = "! Under ATTACK !";

// ===== Runtime variables ===== //
int ch_index { 0 };               // Current index of channel array
int packet_rate { 0 };            // Deauth packet counter (resets with each update)
int attack_counter { 0 };         // Attack counter
unsigned long update_time { 0 };  // Last update time
unsigned long update_time_display { 0 };  // Last update for display
unsigned long ch_time { 0 };      // Last channel hop time

unsigned long total_attack_counter { 0 }; //Total attacks counter
bool led_ext_blink { false };

time_t now;
tm local_tm;
int curHour = 0, curMinute = 0, curDay = 0, curMonth = 0, curYear = 0;


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

String old_msg, old_input;
int old_packet_rate, old_packets_count, old_total_attack_counter, old_hour, old_minute, old_day, old_month, old_year;

void display_string(String input){
  String msg;
  char timeBuffer[11];

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


/*  zrobic refresh tylko wtedy jak cos sie zmienilo. */
if (packet_rate != old_packet_rate || packets_count != old_packets_count || total_attack_counter != old_total_attack_counter)
{
    display.setPartialWindow(100, 150, 100, 50);
      display.firstPage();
      do
      {
//        display.fillRect(6, 125, 100, 20, GxEPD_WHITE);
        display.setTextSize(1);
        display.setCursor(106, 165);
        display.print(String(packets_count));
        display.setCursor(106, 180);
        display.print(String(packet_rate));        
        display.setCursor(106, 195);
        display.print(String(total_attack_counter));        
/*        display.setCursor(106, 190);
        display.print(String(ch)); */
      }
      while (display.nextPage());
}

if (input != old_input)
{
    display.setPartialWindow(0, 38, 200, 15);
      display.firstPage();
      do
      {
        display.setTextSize(1);
        display.getTextBounds(input, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 53);
        display.print(input);
      }
      while (display.nextPage());
}

if (msg != old_msg)
{
    display.setPartialWindow(0, 0, 200, 35);
      display.firstPage();
      do
      {
        display.setTextSize(2);
        display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 30);
        display.print(msg);
      }
      while (display.nextPage());
}
  time(&now);
  localtime_r(&now, &local_tm);
  curHour = local_tm.tm_hour;
  curMinute = local_tm.tm_min;
  curDay = local_tm.tm_mday;
  curMonth = local_tm.tm_mon;
  curYear = local_tm.tm_year;


//test
//  curHour = 4;
//  curMinute = 9;
//  Serial.printf("Time: %02d:%02d\n", curHour, curMinute);

if (curHour != old_hour || curMinute != old_minute)
{
    display.setPartialWindow(0, 65, 200, 55);
      display.firstPage();
      display.setFont(&Outfit_80036pt7b);
      display.setTextSize(1);
      sprintf(timeBuffer, "%02d:%02d", curHour, curMinute);
      Serial.println(timeBuffer);
        display.getTextBounds(timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
      do
      {
        display.setCursor(x, 115);
        display.print(timeBuffer);
      }
      while (display.nextPage());
}
display.setFont(&FreeMonoBold9pt7b);
if (curDay != old_day || curMonth != old_month)
{
    display.setPartialWindow(0, 120, 200, 15);
      display.firstPage();
      display.setTextSize(1);
      sprintf(timeBuffer, "%04d/%02d/%02d", 1900+curYear, 1+curMonth, curDay);
      Serial.println(timeBuffer);
        display.getTextBounds(timeBuffer, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
      do
      {
        display.setCursor(x, 135);
        display.print(timeBuffer);
      }
      while (display.nextPage());
}

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


/*  msg = msg + "    " + String(spin[cc2]) + "\n";
  msg = msg + input + "\n";
  msg = msg + "Pks :" + String(packets_count)+"\n";
  msg = msg + "Pk/s:" + String(packet_rate)+"\n";
  msg2 = "A:" + String(total_attack_counter) + "\n      C:" + String(ch);
*/


/*  display.setCursor(0,0);
  display.println(msg);
  display.setTextSize(2);
  display.setCursor(68,0);
  display.println(msg2);
  display.setTextSize(1); */
/*  display.display();  */
/*  display.clearDisplay(); */
  display.hibernate();
}

// ===== Attack detection functions ===== //
void attack_started() {
  song_playing = true;
  note_index = 0;
  note_time = duration[note_index] * SPEED;
  total_attack_counter++;

  digitalWrite(LED, !LED_INVERT); // turn LED on
  digitalWrite(LED_E, !LED_E_INVERT); // turn LED on
  ATTACK = true;
  packets_count = 0;
  Serial.println(attack_lng);
}

void attack_stopped() {
  song_playing = false;
  noTone(BUZZER); // Stop playing

  digitalWrite(LED, LED_INVERT); // turn LED off
  digitalWrite(LED_E, LED_E_INVERT); // turn LED off
  ATTACK = false;
  Serial.println(scanning_lng);
}


void get_wifi()
{
  WiFi.begin(ssid, password);
  //等待wifi连接
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); //连接成功
  Serial.print("IP address: ");    //打印IP地址
  Serial.println(WiFi.localIP());
}


// ===== Setup ===== //
void setup() {
  String msgt;
//ePaper
  display.init(115200,true,50,false);
  display.setRotation(D_ROTATION);
  display.setFont(&FreeMonoBold9pt7b);
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
    display.setCursor(x, y-tbh);
    display.print(Title);
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    display.getTextBounds(VersionLong, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    display.setCursor(x, y+tbh);
    display.print(VersionLong);
  }
  while (display.nextPage());

  configTime(MY_TZ, MY_NTP_SERVER); 


  Serial.begin(SERIAL_BAUD); // Start serial communication

  pinMode(LED, OUTPUT); // Enable LED pin
  pinMode(LED_E, OUTPUT); // Enable LED_E pin
  digitalWrite(LED, LED_INVERT);
  digitalWrite(LED_E, LED_E_INVERT);

  pinMode(BUZZER, OUTPUT); // Init buzzer pin

  Serial.println();
  Serial.println("Init");


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  /* if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
*/

  get_wifi();
  // Wait for display
  Serial.print("Waiting for NTP sync...");
  now = time(nullptr);
  // Loop until time is > 24h since Jan 1 1970 (1st Jan 2010 onwards)
  int break_loop = 0;
  while (now < 24 * 3600) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    break_loop++;
    if (break_loop > 20) break;
  }
  if (now > 24 * 3600)
    Serial.println("Synced!");
  else
    Serial.println("NTP Sync error!");
  //delay(4000);

  WiFi.disconnect();                   // Disconnect from any saved or active WiFi connections
  wifi_set_opmode(STATION_MODE);       // Set device to client/station mode
  wifi_set_promiscuous_rx_cb(sniffer); // Set sniffer function
  wifi_set_channel(channels[0]);        // Set channel
  wifi_promiscuous_enable(true);       // Enable sniffer

  ////display.display();
  ////delay(1000); // Pause for 2 seconds

  // Clear the buffer
  ////display.clearDisplay();
  ////display.setTextSize(1);
  ////display.setTextColor(WHITE);
    time(&now);
    localtime_r(&now, &local_tm);
    display.setPartialWindow(0, 0, 200, 200);
      display.firstPage();
      do
      {
//        display.fillRect(6, 125, 100, 20, GxEPD_WHITE);
        display.setCursor(6, 165);
        display.print("Packets : 0");
        display.setCursor(6, 180);
        display.print("Packs/s : 0");        
        display.setCursor(6, 195);
        display.print("Attacks : 0");        
        /* display.getTextBounds(VersionShort, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 195);
        display.print(VersionShort);
        */
        display.setTextSize(1);
        display.getTextBounds(scanning_lng, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 53);
        display.print(scanning_lng);
        display.setTextSize(2);
        display.getTextBounds("(o_o)", 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 30);
        display.print("(o_o)");

/*        display.setFont(&Outfit_80036pt7b);
        display.setTextSize(1);
        msgt = printf("%02d:%02d", local_tm.tm_hour, local_tm.tm_min);
        display.getTextBounds(msgt, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display.width() - tbw) / 2) - tbx;
        display.setCursor(x, 112);
        display.print(msgt);
*/
      }
      while (display.nextPage());
  display.setFont(&FreeMonoBold9pt7b);
  display.hibernate();
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


  if (current_time - update_time_display >= 3000) {
    update_time_display = current_time; // Update time variable
    if (ATTACK == true){
          packets_count += int(packet_rate);
          display_string(attack_lng);
          led_ext_blink = !led_ext_blink;
          digitalWrite(LED_E, led_ext_blink); // turn LED off

    }
    else{
          display_string(scanning_lng);
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
