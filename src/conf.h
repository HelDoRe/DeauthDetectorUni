#define DEBUG_SERIAL 1 /* Turn on serial debug output */
#define D_ROTATION 1
#define BASE_FONT Outfit_60011pt7b
#define SMALL_FONT FreeMonoBold9pt7b
#define BIG_FONT Outfit_80036pt7b
#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"

#ifdef DEBUG_SERIAL
#define SERIAL_BAUD 115200 /* Baudrate for serial communication */
#define AP_NAME "DDDD" /* Access Point name for WiFiManager (DEauthDEtectorDEviceDEbug)*/
const char Title[] = "DeauthDetectorD";
const char VersionLong[] = "v0.0.4 (20260208)";
const char VersionShort[] = "v0.0.4d";
#else
#define SERIAL_BAUD 0 /* Turn off Serial */
#define AP_NAME "DeDeDe" /* Access Point name for WiFiManager (DEauthDEtectorDEvice)*/
const char Title[] = "DeauthDetector";
const char VersionLong[] = "v0.0.4";
const char VersionShort[] = "v0.0.4";
#endif