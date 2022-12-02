/**
 * @digitaldragon/WiFi_FirmwareUpdater
 * 
 * @example An example code for esp32 chipset. Check against a firmware
 * version file for a new firmware version.  If one exists the binary
 * will be downloaded and streamed into an update buffer.
 * 
 * This example supports up to 3 sets of WiFi credentials, an error
 * message can be extracted from the WiFi_FirmwareUpdater if an
 * error occurs in process an update.
 * 
 * The library provide a high level API so this
 * process is handled in 2 function calls.
 * 
*/
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi_FirmwareUpdater.h>

#define BAUD_RATE 115200

const char *ssid1 = "Home WiFi";
const char *wifiPassword2 = "k54feSIWS93923092kzx";
const char *ssid2 = "Garage WiFi";
const char *wifiPassword2 = "8dhfgdgY23fsfJHfjhif";
const char *ssid3 = "";
const char *wifiPassword3 = "";

const char *firmwareVersion = "0.1.1";

unsigned long connectionInterval = 0;
unsigned long previousMillis = 0;

const char *updateBinaryUrl = "https://example.server.com/firmware.bin"; // must include either http:// or https://
const char *versionFileUrl = "https://example.server.com/firmware.txt";

WiFi_Credentials keys {
  ssid1,
  wifiPassword2,
  ssid2,
  wifiPassword2,
  ssid3,
  wifiPassword3
};

WiFi_FirmwareUpdater update;
HTTPClient client;

void setup()
{
  Serial.begin(BAUD_RATE);
  delay(1000);
  Serial.printf("[Current Firmware Version] %s\n", firmwareVersion);

  update.configure(keys, firmwareVersion);
}

void loop()
{
  if (update.checkUpdateAvailable(client, versionFileUrl)) {
    Serial.print("[Connected at IP Address] ");
    Serial.println(update.ipAddress());
    delay(3);
    Serial.print("[Firmware version available] ");
    Serial.println(update.availableFirmwareVersion());

    update.updateFirmware(client, updateBinaryUrl);
  } else {
    Serial.println(update.errorString());
  }
  
  while (true) { delay(1); }
}
