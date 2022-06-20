#ifndef WIFI_FIRMWARE_UPDATER_H
#define WIFI_FIRMWARE_UPDATER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "string.h"

class WiFi_FirmwareUpdater: public HTTPClient
{
public:
    const char* ssid; // Local network name.
    const char* password; // Local network password.
    const std::string &currentVersion; // Current firmware version.

    WiFi_FirmwareUpdater(const char* ssid, const char* password, const std::string &currentVersion);
    ~WiFi_FirmwareUpdater();

    bool checkUpdateAvailable(const char *versionFileUrl);
    void updateFirmware(const char *updateUrl);
    const char* getAvailableFirmwareVersion();

private:
    int totalLength; // Total size of firmware.
    int currentLength; // Current size of the written firmware.
    int respCode; // HTTP response from GET requests
    const char* availableVersion; // Firmware version available on the remote server

    void connectWifi();
    void getRequest(const char *url);
    int getVersionNumberFromString(bool currentVersionCheck);
    void processUpdate(uint8_t *data, size_t len);
};

#endif
