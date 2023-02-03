#include "WiFi_FirmwareUpdater.h"

// Constructors
WiFi_FirmwareUpdater::WiFi_FirmwareUpdater() {}

// Destructor
WiFi_FirmwareUpdater::~WiFi_FirmwareUpdater() {}

void WiFi_FirmwareUpdater::configure(WiFi_Credentials &credentials, const char * firmwareVersion)
{
    _ssid_1 = credentials.ssid_1;
    _password_1 = credentials.pass_1;
    _ssid_2 = credentials.ssid_2;
    _password_2 = credentials.pass_2;
    _ssid_3 = credentials.ssid_3;
    _password_3 = credentials.pass_3;
    _currentVersion = firmwareVersion;
}

void WiFi_FirmwareUpdater::configure(const char * ssid, const char * password, const char * firmwareVersion)
{
  _ssid_1 = std::string(ssid);
  _password_1 = std::string(password);
  _currentVersion = firmwareVersion;
}

/**
 * @brief Start WiFi connection, allows 20 seconds to make a connection, before
 * setting a failed to connect errorNumber.
 * 
 * @param credential int
 * @return bool
 */
bool WiFi_FirmwareUpdater::connectWifi(int credential) // private
{
  if (WiFi.mode(WIFI_MODE_STA)) {
    _errorNumber = 0;
  } 

  int status = WL_CONNECTED;

  if (credential == 1) {
    status = WiFi.begin(_ssid_1.c_str(), _password_1. c_str());
  } else if (credential == 2) {
    status = WiFi.begin(_ssid_2.c_str(), _password_2.c_str());
  } else if (credential == 3) {
    status = WiFi.begin(_ssid_3.c_str(), _password_3.c_str());
  }

  int maxWait = 20;

  while (status != WL_CONNECTED) {
    maxWait -= 1;
    if (maxWait == 0) {
      break;
    }
    status = WiFi.status();
    delay(500);
  }

  if (!WiFi.isConnected()) {
    if (credential == 1 && _ssid_2 != "" && _password_2 != "") {
      this->connectWifi(2);
    } else if (credential == 2 && _ssid_3 != "" && _password_3 != "") {
      this->connectWifi(3);
    } else {
      _errorNumber = 1;
      return false;
    }
  }
  
  return true;
}

/**
 * @brief Get the firmware version available on the update server.
 * 
 * @return String
 */
std::string WiFi_FirmwareUpdater::availableFirmwareVersion() // public
{
  return _availableVersion;
}

/**
 * @brief Get the most recent error number from the class.
 * 
 * @return int
 */
int WiFi_FirmwareUpdater::error()
{
  return _errorNumber;
}

/**
 * @brief get the errorNumber message from the errorNumber number.
 * 
 * @return const char *
 */
const char * WiFi_FirmwareUpdater::errorString() // public
{
  if (_errorNumber == FIRMWARE_ERROR_OK) {
    return ("No error");
  } else if (_errorNumber == FIRMWARE_ERROR_WIFI) {
    return ("Failed to connect to WiFi");
  } else if (_errorNumber == FIRMWARE_ERROR_RESPONSE) {
    return ("Only HTTP response 200 is supported");
  } else if (_errorNumber == FIRMWARE_ERROR_UPDATE_BEGIN) {
    return ("Update class couldn't begin update, not enough space for update");
  } else if (_errorNumber == FIRMWARE_ERROR_NO_CURRENT_VERSION) {
    return ("Failed to read the current firmware version");
  } else if (_errorNumber == FIRMWARE_ERROR_NO_UPDATE_VERSION) {
    return ("Failed to read the updates firmware version");
  } else if (_errorNumber == FIRMWARE_ERROR_CONSTRUCTOR) {
    return ("Incorrect constructor, a HardwareSerial object must be passed in to use hexDump()");
  }
  return ("Unknown error");
}

/**
 * @brief use the WiFi class to return the local IP Address
 * 
 * @return IPAddress
 */
IPAddress WiFi_FirmwareUpdater::ipAddress() // public
{
  return WiFi.localIP();
}

/**
 * @brief hexDump(desc, addr, len, perLine);
 * desc:    if non-NULL, printed as a description before hex dump.
 * addr:    the address to start dumping from.
 * len:     the number of bytes to dump.
 * perLine: number of bytes on each output line.
 * 
 * @param desc 
 * @param addr 
 * @param len 
 * @param perLine 
 * 
 * @return void
 */
void WiFi_FirmwareUpdater::hexDump (
    HardwareSerial &Serial,
    const char * desc,
    const void * addr,
    const int len,
    int perLine
) {
    // Silently ignore silly per-line values.
    // if (perLine < 4 || perLine > 64) perLine = 16;
    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.
    if (desc != "")
       log_i("%s:\n", desc);

    // Length checks.
    if (len == 0)
        log_w("  ZERO LENGTH\n");
        return;

    if (len < 0)
        log_w("  NEGATIVE LENGTH: %d\n", len);
        return;

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).
        if ((i % perLine) == 0)
            // Only print previous-line ASCII buffer for lines beyond first.
            if (i != 0) 
                Serial.printf("  %s\n", buff);
            // Output the offset of current line.
            Serial.printf("  %04x ", i);

        // Now the hex code for the specific character.
       Serial.printf(" %02x", pc[i]);

        // And buffer a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.
    while ((i % perLine) != 0) {
        Serial.printf("   ");
        i++;
    }

    // And print the final ASCII buffer.
    Serial.printf ("  %s\n", buff);
}
