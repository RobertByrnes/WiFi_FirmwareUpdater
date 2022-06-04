#include "WiFi_FirmwareUpdater.h"

// Constructor
WiFi_FirmwareUpdater::WiFi_FirmwareUpdater(const char* ssid, const char* password, const std::string &currentVersion):
   ssid(ssid), 
   password(password), 
   totalLength(0), 
   currentLength(0),
   currentVersion(currentVersion) {}

// Destructor
WiFi_FirmwareUpdater::~WiFi_FirmwareUpdater() {}

/**
 * @brief Connects to the update server and gets the version file,
 * if the version is greater that the current version will
 * return true else false.
 * 
 * @return bool
 */
bool WiFi_FirmwareUpdater::checkUpdateAvailable(const char *verisonFileUrl)
{
  this->connectWifi();
  this->getRequest(verisonFileUrl);

  if (this->respCode == 200) {
    int len = totalLength = this->getSize(); // get length of doc (is -1 when Server sends no Content-Length header)
    Serial.println("[i] Checking firmware versions...");
    int currentVersion = this->getVersionNumberFromString(true);
    int availableVersion = this->getVersionNumberFromString(false);
    Serial.printf("[i] Current firmware version: %u\n", currentVersion);
    Serial.printf("[i] Firmware version available: %u\n", availableVersion);
    Serial.println("[i] Comparing versions...");

    if (availableVersion > currentVersion) {
      Serial.println("[i] Firmware upgrade available, will download");
    } else {
      Serial.println("[i] The current Firware version is the latest version");
    }
  
  } else {
    Serial.print("[!] Memory: ");
    Serial.println(ESP.getMaxAllocHeap());
    Serial.println("[-] Cannot download firmware version file. Only HTTP response 200 is supported.");  
    this->end(); 
    while (!WiFi.disconnect()) {
      sleep(1);
    };
    return false;
  }
  
  this->end(); 
  while (!WiFi.disconnect()) {
    sleep(1);
  };
  return true;
}

/**
 * @brief Start WiFi connection.
 * 
 * @return void
 */
void WiFi_FirmwareUpdater::connectWifi() // private
{
    WiFi.mode(WIFI_MODE_STA);        
    WiFi.begin(this->ssid, this->password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("[+] WiFi connected");
    Serial.print("[i] Local IP: ");
    Serial.println(WiFi.localIP());
}

/**
 * @brief Connect to update server with a GET request.
 * 
 * @return void
 */
void WiFi_FirmwareUpdater::getRequest(const char *url)
{
  this->begin(url);
  try {
    this->respCode = this->GET();
    std::string errorString = "Underlying library issue reading socket";
    if (this->respCode != 200) throw errorString;
  } catch (std::string error) {
    Serial.print("[!] Error caught from GET request, attempting to continue");
    this->respCode = 200;
  }
  Serial.print("[i] Response: ");
  Serial.println(respCode);
}

/**
 * @brief Parses the verion number (sematic versioning) from a string read
 * from the GET request, or from the CURRENT_VERSION decalred in 
 * version.h. The version is returned as integer for comparison,
 * e.g. "version=1.2.8" will be returned as 128.
 * 
 * @return int
 */
int WiFi_FirmwareUpdater::getVersionNumberFromString(bool currentVersionCheck) 
{
  std::string version;

  if (currentVersionCheck == false) {
    version = this->getString().c_str();
    // Serial.print("[D] std::string version: "); Serial.println(version.c_str());
    version = version.substr(version.find_first_of("=") + 1);
    // Serial.print("[D] std::string version number: "); Serial.println(version.c_str());
  } else {
    version = this->currentVersion;
  }

  std::string output;
  output.reserve(version.size()); // optional, avoids buffer reallocations in the loop
  for (size_t i = 0; i < version.size(); ++i) {
    if (version[i] != '.') {
      output += version[i];
    }
  }
  // Serial.print("[D] std::string output: "); Serial.println(output.c_str());
  return std::atoi( output.c_str() );
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
  this->currentLength += len;
  // Print dots while waiting for update to finish
  Serial.print('.');
  // if current length of written firmware is not equal to total firmware size, repeat
  if (this->currentLength != totalLength) return;
  Update.end(true);
  Serial.printf("\n[+] Update Success, Total Size: %u\nRebooting...\n", this->currentLength); 
  WiFi.disconnect();
  // Restart ESP32 to see changes
  ESP.restart();
}

/**
 * @brief Begins the update process: this is done by creating a 
 * 128 byte buffer to the read the binary firmware file into in
 * 128 byte chunks. Each chunk is then passed to the
 * processUpdate function to be written to flash.
 * 
 * @return void
 */
void WiFi_FirmwareUpdater::updateFirmware(const char *updateUrl)
{
  this->connectWifi();
  this->getRequest(updateUrl);

  if (this->respCode == 200) {
      int len = totalLength = this->getSize(); // get length of doc (is -1 when Server sends no Content-Length header)
      Update.begin(UPDATE_SIZE_UNKNOWN);
      Serial.printf("[i] Firmware Size: %u\n",totalLength);
      uint8_t buff[128] = { 0 }; // create buffer for read
      WiFiClient * stream = this->getStreamPtr(); // get tcp stream
      Serial.println("[i] Updating firmware...");
      
      while(this->connected() && (len > 0 || len == -1)) { // read all data from server
        size_t size = stream->available(); // get available data size
        if (size) {
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size)); // read up to 128 byte
          processUpdate(buff, c);

          if (len > 0) {
              len -= c;
          }
        }
        delay(1);
      }
  } else {
    Serial.println("[-] Cannot download firmware file. Only HTTP response 200 is supported.");
  }
  this->end();
}
