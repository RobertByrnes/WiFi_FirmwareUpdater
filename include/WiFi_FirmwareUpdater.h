#pragma once

#ifndef WIFI_FIRMWARE_UPDATER_H
#define WIFI_FIRMWARE_UPDATER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "string.h"
#include <HardwareSerial.h>

using namespace std;

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

class WiFi_FirmwareUpdater
{
public:
    std::string _ssid_1 = ""; // Local network name
    std::string _password_1 = ""; // Local network password
    std::string _ssid_2 = ""; // Local network name
    std::string _password_2 = ""; // Local network password
    std::string _ssid_3 = ""; // Local network name
    std::string _password_3 = ""; // Local network password
    const char *_httpErrorString;
    std::string _currentVersion = ""; // Current firmware version
    uint8_t _errorNumber = 0; // Error number
    HardwareSerial _serial = 0;

    WiFi_FirmwareUpdater();
    ~WiFi_FirmwareUpdater();

    void configure(WiFi_Credentials &credentials, const char * firmwareVersion);
    void configure(const char * ssid, const char * password, const char * firmwareVersion);

    bool connectWifi(int credential = 1);
    String availableFirmwareVersion();
    const char * errorString();
    int error();
    IPAddress ipAddress();

    /**
     * @brief Connect to the update server and get the version file,
     * if the version is greater than the current version this 
     * function will return true, else it will be false.
     * 
     * @param versionFileUrl const char *
     * @return bool
     */
    template <typename ClientType>
    bool checkUpdateAvailable(ClientType &client, const char * versionFileUrl)
    {
        if (!this->connectWifi(1) || !this->getRequest(client, versionFileUrl)) {
            return false;
        }

        if (_respCode == 200) {
            int len = _totalLength = client.getSize(); // get length of doc (is -1 when Server sends no Content-Length header)
            int currentVersion = this->versionNumberFromString(client, true);
            int availableVersion = this->versionNumberFromString(client, false);

            if (availableVersion > currentVersion) {
                return true;
            } else {
                return false;
            }
        } else {
            _errorNumber = 2;

            client.end(); 
            
            while (!WiFi.disconnect()) {
                sleep(1);
            };

            return false;
        }

        return true;
    }

    /**
     * @brief Begins the update process: this is done by creating a 
     * 128 byte buffer to the read the binary firmware file into in
     * 128 byte chunks. Each chunk is then passed to the
     * processUpdate function to be written to flash.
     * 
     * If this function fails the errorNumber is returned, call
     * WiFi_FirmwareUpdater::errorString to retrieve the errorNumber message.
     * 
     * @param client typename ClientType
     * @param updateUrl const char * 
     * @param hexDump uint8_t
     * @return void
     */
    template <typename ClientType>
    int updateFirmware(ClientType &client, const char * updateUrl, uint8_t hexDump = 0)
    {
        if (hexDump != 0 && _serial == 0) {
            _errorNumber = 4;
            return _errorNumber;
        }

        if (!this->connectWifi(1)) {
            return _errorNumber;
        }

        if (!this->getRequest(client, updateUrl)) {
            return _errorNumber;
        }

        int len = _totalLength = client.getSize(); // get the value of the content-length header

        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            _errorNumber = 3;
            return _errorNumber;
        }
        
        uint8_t buff[128] = { 0 }; // create buffer for read
        WiFiClient * stream = client.getStreamPtr(); // get tcp stream
        
        int loopCount = 0;

        while (client.connected() && (len > 0 || len == -1)) { // read all data from server
            size_t size = stream->available(); // get available data size
            
            if (size) {
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size)); // read up to 128 bytes into the buffer

                if (loopCount == 0 && hexDump != 0) {
                    this->hexDump(_serial, "[DUMP buffer]", &buff, 128, 16); // remove serial a member variable anyway
                    this->hexDump(_serial, "[DUMP c]", &c, 128, 16);
                }

                loopCount = 1;

                try {
                    processUpdate(client, buff, c);
                } catch (int error) {
                    _errorNumber = error;
                    return _errorNumber;
                }

                if (len > 0) {
                    len -= c;
                }
            }
            delay(1);
        }

        return 0;
    }

private:
    int _totalLength; // Total size of firmware
    int _currentLength; // Current size of the written firmware
    int _respCode; // HTTP response from GET requests
    String _availableVersion; // Firmware version available on the remote server
    
    void hexDump(HardwareSerial &Serial, const char * desc, const void * addr, const int len, int perLine);

    /**
     * @brief Connect to update server with a GET request.
     * 
     * @param client typename ClientType
     * @param url const char *
     * @return bool
     */
    template <typename ClientType>
    bool getRequest(ClientType &client, const char * url)
    {
        client.begin(url);
        _respCode = client.GET();

        if (_respCode != 200) {
            _errorNumber = 2;
            return false;
        }

        return true;
    }

    /**
     * @brief Function to update firmware incrementally
     * Buffer is declared to be 128 so chunks of 128 bytes
     * from firmware is written to device until server closes.
     * 
     * @param data uint8_t *
     * @param len size_t
     * 
     * @return void
     */ 
    template <typename ClientType>
    void processUpdate(ClientType &client, uint8_t *data, size_t len)
    {
        Update.write(data, len);
        _currentLength += len;
        // if current length of written firmware is not equal to total firmware size, repeat
        if (_currentLength != _totalLength) return;

        if (!Update.end(true)) {
            throw 7;
        }

        client.end();
        WiFi.disconnect();
        ESP.restart();
    }

    /**
     * @brief Parses the verion number (sematic versioning) from a string read
     * from the GET request, or from the CURRENT_VERSION decalred in 
     * version.h. The version is returned as integer for comparison,
     * e.g. "version=1.2.8" will be returned as 128.
     * 
     * @param currentVersionCheck bool
     * @return int version or int error
     */
    template <typename ClientType>
    int versionNumberFromString(ClientType &client, bool currentVersionCheck)
    {
        std::string version;

        try {
            if (currentVersionCheck == false) {
                version = client.getString().c_str();
                version = version.substr(version.find_first_of("=") + 1);
                _availableVersion = version.c_str();

                if (_availableVersion == "") {
                    throw 5;
                }

                if (version == "") {
                    throw 6;
                } 
            } else {
                version = _currentVersion;
            }
        } catch (int error) {
            _errorNumber = error;
            return error;
        }

        std::string output;
        output.reserve(version.size()); // optional, avoids buffer reallocations in the loop

        for (size_t i = 0; i < version.size(); ++i) {
            if (version[i] != '.') {
                output += version[i];
            }
        }
        return std::atoi( output.c_str() );
    }

};

#endif
