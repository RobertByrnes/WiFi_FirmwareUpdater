#include "WiFi_FirmwareUpdater.h"
#include "hexdump.cpp"

// Constructors
WiFi_FirmwareUpdater::WiFi_FirmwareUpdater(WiFi_Credentials credentials, const char *currentVersion):
  ssid_1(credentials.ssid_1), 
  password_1(credentials.pass_1),
  ssid_2(credentials.ssid_2), 
  password_2(credentials.pass_2), 
  ssid_3(credentials.ssid_3), 
  password_3(credentials.pass_3),  
  totalLength(0), 
  currentLength(0),
  currentVersion(currentVersion) {}

WiFi_FirmwareUpdater::WiFi_FirmwareUpdater(WiFi_Credentials credentials, const char *currentVersion, HardwareSerial Serial):
  ssid_1(credentials.ssid_1), 
  password_1(credentials.pass_1),
  ssid_2(credentials.ssid_2), 
  password_2(credentials.pass_2), 
  ssid_3(credentials.ssid_3), 
  password_3(credentials.pass_3),  
  totalLength(0), 
  currentLength(0),
  currentVersion(currentVersion)
{
  this->serial = Serial;
}

WiFi_FirmwareUpdater::WiFi_FirmwareUpdater(const char* ssid, const char* password, const char *currentVersion):
  ssid_1(ssid), 
  password_1(password), 
  totalLength(0), 
  currentLength(0),
  currentVersion(currentVersion) {}


WiFi_FirmwareUpdater::WiFi_FirmwareUpdater(const char* ssid, const char* password, const char *currentVersion, HardwareSerial Serial):
  ssid_1(ssid), 
  password_1(password), 
  totalLength(0), 
  currentLength(0),
  currentVersion(currentVersion)
{
  this->serial = Serial;
}

// Destructor
WiFi_FirmwareUpdater::~WiFi_FirmwareUpdater() {}

/**
 * @brief Get the firmware version available on the update server.
 * 
 * @return String
 */
String WiFi_FirmwareUpdater::availableFirmwareVersion() // public
{
  return this->availableVersion;
}

/**
 * @brief Connect to the update server and get the version file,
 * if the version is greater than the current version this 
 * function will return true, else it will be false.
 * 
 * @param versionFileUrl const char *
 * 
 * @return bool
 */
bool WiFi_FirmwareUpdater::checkUpdateAvailable(const char *verisonFileUrl) // public
{
  if (!this->connectWifi(1) || !this->getRequest(verisonFileUrl)) {
    return false;
  }

  if (respCode == 200) {
    int len = this->totalLength = this->getSize(); // get length of doc (is -1 when Server sends no Content-Length header)
    int currentVersion = this->versionNumberFromString(true);
    int availableVersion = this->versionNumberFromString(false);

    if (availableVersion > currentVersion) {
      return true;
    } else {
      return false;
    }
  } else {
    this->errorNumber = 2;

    this->end(); 
    
    while (!WiFi.disconnect()) {
      sleep(1);
    };

    return false;
  }

  return true;
}

/**
 * @brief Start WiFi connection, allows 20 seconds to make a connection, before
 * setting a failed to connect errorNumber.
 * 
 * @return bool
 */
bool WiFi_FirmwareUpdater::connectWifi(int credential) // private
{
  if (WiFi.mode(WIFI_MODE_STA)) {
    this->errorNumber = 0;
  } 

  int status = WL_CONNECTED;

  if (credential == 1) {
    status = WiFi.begin(this->ssid_1, this->password_1);
  } else if (credential == 2) {
    status = WiFi.begin(this->ssid_2, this->password_2);
  } else if (credential == 3) {
    status = WiFi.begin(this->ssid_3, this->password_3);
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
    if (credential == 1 && this->ssid_2 != NO_CREDENTIALS && this->password_2 != NO_CREDENTIALS) {
      this->connectWifi(2);
    } else if (credential == 2 && this->ssid_3 != NO_CREDENTIALS && this->password_3 != NO_CREDENTIALS) {
      this->connectWifi(3);
    } else {
      this->errorNumber = 1;
      return false;
    }
  }
  
  return true;
}

/**
 * @brief Get the most recent error number from the class.
 * 
 * @return int
 */
int WiFi_FirmwareUpdater::error()
{
  return this->errorNumber;
}

/**
 * @brief get the errorNumber message from the errorNumber number.
 * 
 * @param errorNumber uint8_t
 * 
 * @return const char *
 */
const char * WiFi_FirmwareUpdater::errorString() // public
{
  if (this->errorNumber == FIRMWARE_ERROR_OK) {
    return ("No error");
  } else if (this->errorNumber == FIRMWARE_ERROR_WIFI) {
    return ("Failed to connect to WiFi");
  } else if (this->errorNumber == FIRMWARE_ERROR_RESPONSE) {
    return ("Only HTTP response 200 is supported");
  } else if (this->errorNumber == FIRMWARE_ERROR_UPDATE_BEGIN) {
    return ("Update class couldn't begin update, not enough space for update");
  } else if (this->errorNumber == FIRMWARE_ERROR_NO_CURRENT_VERSION) {
    return ("Failed to read the current firmware version");
  } else if (this->errorNumber == FIRMWARE_ERROR_NO_UPDATE_VERSION) {
    return ("Failed to read the updates firmware version");
  } else if (this->errorNumber == FIRMWARE_ERROR_CONSTRUCTOR) {
    return ("Incorrect constructor, a HardwareSerial object must be passed in to use hexDump()");
  }
  return ("Unknown error");
}

/**
 * @brief Connect to update server with a GET request.
 * 
 * @return bool
 */
bool WiFi_FirmwareUpdater::getRequest(const char *url) // private
{
  this->begin(url);
  respCode = this->GET();

  if (respCode != 200) {
    errorNumber = 2;
    return false;
  }

  return true;
}

// Usage:
//     hexDump(desc, addr, len, perLine);
//         desc:    if non-NULL, printed as a description before hex dump.
//         addr:    the address to start dumping from.
//         len:     the number of bytes to dump.
//         perLine: number of bytes on each output line.

void WiFi_FirmwareUpdater::hexDump(HardwareSerial &Serial, const char * desc, const void * addr, const int len, int perLine)
{
    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;

    if (desc != "") {
      Serial.printf("%s:\n", desc);
    }
    
    if (len == 0) {
        Serial.print("  ZERO LENGTH\n");
        return;
    }

    if (len < 0) {
        Serial.printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.
            if (i != 0) {
              Serial.printf("  %s\n", buff);
            }
            // Output the offset of current line.
            Serial.printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        Serial.printf(" %02x", pc[i]);

        // And buffer a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
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
    Serial.printf("  %s\n", buff);
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
 * @brief Function to update firmware incrementally
 * Buffer is declared to be 128 so chunks of 128 bytes
 * from firmware is written to device until server closes.
 * 
 * @param data uint8_t *
 * @param len size_t
 * 
 * @return void
 */ 
void WiFi_FirmwareUpdater::processUpdate(uint8_t *data, size_t len) // private
{
  Update.write(data, len);
  currentLength += len;
  // if current length of written firmware is not equal to total firmware size, repeat
  if (currentLength != totalLength) return;

  if (!Update.end(true)) {
    throw 7;
  }

  this->end();
  WiFi.disconnect();
  ESP.restart();
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
 * @param updateUrl const char * 
 * 
 * @return void
 */
int WiFi_FirmwareUpdater::updateFirmware(const char *updateUrl, uint8_t hexDump)
{
  if (hexDump != 0 && this->serial == 0) {
    this->errorNumber = 4;
    return this->errorNumber;
  }

  if (!this->connectWifi(1)) {
    return this->errorNumber;
  }

  if (!this->getRequest(updateUrl)) {
    return this->errorNumber;
  }

  int len = totalLength = this->getSize(); // get the value of the content-length header

  if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
    errorNumber = 3;
    return this->errorNumber;
  }
  
  uint8_t buff[128] = { 0 }; // create buffer for read
  WiFiClient * stream = this->getStreamPtr(); // get tcp stream
  
  int loopCount = 0;

  while (this->connected() && (len > 0 || len == -1)) { // read all data from server
    size_t size = stream->available(); // get available data size
    
    if (size) {
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size)); // read up to 128 bytes into the buffer

      if (loopCount == 0 && hexDump != 0) {
        this->hexDump(this->serial, "[DUMP buffer]", &buff, 128, 16); // remove serial a member variable anyway
        this->hexDump(this->serial, "[DUMP c]", &c, 128, 16);
      }

      loopCount = 1;

      try {
        processUpdate(buff, c);
      } catch (int error) {
        this->errorNumber = error;
        return this->errorNumber;
      }

      if (len > 0) {
          len -= c;
      }
    }
    delay(1);
  }

  return 0;
}

/**
 * @brief Parses the verion number (sematic versioning) from a string read
 * from the GET request, or from the CURRENT_VERSION decalred in 
 * version.h. The version is returned as integer for comparison,
 * e.g. "version=1.2.8" will be returned as 128.
 * 
 * @return int version or int error
 */
int WiFi_FirmwareUpdater::versionNumberFromString(bool currentVersionCheck) // private
{
  std::string version;

  try {
    if (currentVersionCheck == false) {
      version = this->getString().c_str();
      version = version.substr(version.find_first_of("=") + 1);
      this->availableVersion = version.c_str();

      if (this->availableVersion == "") {
        throw 5;
      }

      if (version == "") {
        throw 6;
      } 
    } else {
      version = this->currentVersion;
    }
  } catch (int error) {
    this->errorNumber = error;
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
