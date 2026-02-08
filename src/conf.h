/* turn on/off feature (debug, buzzer, onboard led, external led) */
#define DEBUG_SERIAL 1 /* Turn on serial debug output */
#define BUZZER D6 /* Buzzer pin */
#define LED 2         /* LED pin (2=built-in LED) */
#define LED_E D3      // D3 (0),  D4 (GPIO2) /* External LED pin */
#define SYNC_NTP 1    /* Sync time with NTP server at startup */
#define USE_DISPLAY 1 /* Turn on display */

#define GLOBAL_LANG 1                /* Language for display (1 = en, 2 = pl, ...) - more can be added in lang/ folder */
#define DISPLAY_TYPE 1               /* Display type: 1 = "EPD_154", 2 = "OLED_096" */
#define D_ROTATION 1                 /* Display orientation */
#define BASE_FONT Outfit_60011pt7b   /* Primary font */
#define SMALL_FONT FreeMonoBold9pt7b /* Small info font */
#define BIG_FONT Outfit_80036pt7b    /* Big clock font */

#ifdef SYNC_NTP
#define MY_NTP_SERVER "at.pool.ntp.org"        /* NTP server */
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" /* Timezone (Central European Time) */
#endif

#ifdef LED
#define LED_INVERT true /* Invert HIGH/LOW for LED */
#endif
#ifdef LED_E
#define LED_E_INVERT false /* Invert HIGH/LOW for LED */
#endif
#define CH_TIME 140 /* Scan time (in ms) per channel */
#define PKT_RATE 7  /* Min. packets before it gets recognized as an attack */
#define PKT_TIME 1  /* Min. interval (CH_TIME*CH_RANGE) before it gets recognized as an attack */
// Channels to scan on (US=1-11, EU=1-13, JAP=1-14)
const short channels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 /*,14*/};
// const String spin = "-\\|/";

#ifdef DEBUG_SERIAL
#ifndef SERIAL_BAUD
#define SERIAL_BAUD 115200 /* Baudrate for serial communication */
#endif
#ifdef SYNC_NTP
#define AP_NAME "DDDD" /* Access Point name for WiFiManager (DEauthDEtectorDEviceDEbug)*/
#endif
const char Title[] = "DeauthDetectorD";
const char VersionLong[] = "v0.0.4 (20260208)";
const char VersionShort[] = "v0.0.4d";
#else
#define SERIAL_BAUD 0 /* Turn off Serial */
#ifdef SYNC_NTP
#define AP_NAME "DeDeDe" /* Access Point name for WiFiManager (DEauthDEtectorDEvice)*/
#endif
const char Title[] = "DeauthDetector";
const char VersionLong[] = "v0.0.4";
const char VersionShort[] = "v0.0.4";
#endif
