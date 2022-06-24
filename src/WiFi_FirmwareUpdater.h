#ifndef WIFI_FIRMWARE_UPDATER_H
#define WIFI_FIRMWARE_UPDATER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "string.h"
#include <HardwareSerial.h>

using namespace std;

#define NO_CREDENTIALS                      "Not Set"
#define HEXDUMP                             (1)

#define FIRMWARE_ERROR_OK                   (0)
#define FIRMWARE_ERROR_WIFI                 (1)
#define FIRMWARE_ERROR_RESPONSE             (2)
#define FIRMWARE_ERROR_UPDATE_BEGIN         (3)
#define FIRMWARE_ERROR_CONSTRUCTOR          (4)
#define FIRMWARE_ERROR_NO_CURRENT_VERSION   (5)
#define FIRMWARE_ERROR_NO_UPDATE_VERSION    (6)
#define FIRMWARE_ERROR_UPDATE_END           (7)

typedef struct {
    const char *ssid_1;
    const char *pass_1;
    const char *ssid_2;
    const char *pass_2;
    const char *ssid_3;
    const char *pass_3;
} WiFi_Credentials;

class WiFi_FirmwareUpdater: public HTTPClient
{
public:
    const char *ssid_1; // Local network name
    const char *password_1; // Local network password
    const char *ssid_2 = NO_CREDENTIALS; // Local network name
    const char *password_2 = NO_CREDENTIALS; // Local network password
    const char *ssid_3 = NO_CREDENTIALS; // Local network name
    const char *password_3 = NO_CREDENTIALS; // Local network password
    const char *httpErrorString;
    const string currentVersion; // Current firmware version
    uint8_t errorNumber = 0; // Error number
    HardwareSerial serial = 0;

    WiFi_FirmwareUpdater(WiFi_Credentials credentials, const char *currentVersion);
    WiFi_FirmwareUpdater(WiFi_Credentials credentials, const char *currentVersion, HardwareSerial Serial);
    WiFi_FirmwareUpdater(const char *ssid, const char *password, const char *currentVersion);
    WiFi_FirmwareUpdater(const char *ssid, const char *password, const char *currentVersion, HardwareSerial Serial);
    ~WiFi_FirmwareUpdater();

    bool checkUpdateAvailable(const char *versionFileUrl);
    bool connectWifi(int credential = 1);
    String availableFirmwareVersion();
    const char * errorString();
    int error();
    int updateFirmware(const char *updateUrl, uint8_t hexDump = 0);
    IPAddress ipAddress();

private:
    int totalLength; // Total size of firmware
    int currentLength; // Current size of the written firmware
    int respCode; // HTTP response from GET requests
    String availableVersion; // Firmware version available on the remote server

    bool getRequest(const char *url);
    void hexDump(HardwareSerial &Serial, const char * desc, const void * addr, const int len, int perLine);
    void processUpdate(uint8_t *data, size_t len);
    int versionNumberFromString(bool currentVersionCheck);
};

#endif
