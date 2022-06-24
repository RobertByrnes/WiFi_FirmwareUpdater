#ifndef CONFIG_H
#define CONFIG_H

#include <string>
using namespace std;

// Version
const char *CURRENT_VERSION = "0.0.1"; 

#define BAUD_RATE 115200

// Set the timer interval
// #define CONNECTION_INTERVAL 3600000L // every hour
// #define CONNECTION_INTERVAL 3000000L // every 50mins
// #define CONNECTION_INTERVAL 2400000L // every 40mins
// #define CONNECTION_INTERVAL 1800000L // every 30mins
// #define CONNECTION_INTERVAL 1200000L // every 20mins
// #define CONNECTION_INTERVAL 900000L // every 15mins
// #define CONNECTION_INTERVAL 600000L // every 10mins
// #define CONNECTION_INTERVAL 300000L // every 5mins
// #define CONNECTION_INTERVAL 180000L // every 3mins
// #define CONNECTION_INTERVAL 120000L // every 2mins
#define CONNECTION_INTERVAL 60000L // every 1min

// location of firmware file on external web server
// change to your actual .bin location
const char *UPDATE_HOST = "exaample.server.com";
const char *UPDATE_URL = "https://exaample.server.com/firmware.bin"; // must include either http:// or https://
const char *UPDATE_VERSION_FILE_URL = "https://exaample.server.com/firmware.txt";

// WiFi credentials
const char *SSID_1 = "ssid"; // WiFi Network Name
const char *PASSWORD_1 = "password"; // WiFi Network Password
const char *SSID_2 = "ssid"; // WiFi Network Name
const char *PASSWORD_2 = "password"; // WiFi Network Password
const char *SSID_3 = "ssid"; // WiFi Network Name
const char *PASSWORD_3 = "password"; // WiFi Network Password

// Counter
unsigned long PREVIOUS_MILLIS = 0;

#endif