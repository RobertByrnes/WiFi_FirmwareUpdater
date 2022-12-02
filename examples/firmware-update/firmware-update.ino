/**
 * @digitaldragon/WiFi_FirmwareUpdater
 * 
 * @example A Basic example code for esp32 chipset. Check against a firmware
 * version file for a new firmware version.  If one exists the binary
 * will be downloaded and streamed into an update buffer.
 * 
 * The library provide a high level API so this
 * process is handled in 2 function calls.
 * 
*/

#include <HTTPClient.h>
#include <WiFi_FirmwareUpdater.h>

#define BAUD_RATE 115200

const char *ssid = "Home WiFi";
const char *wifiPassword = "k54feSIWS93923092kzx";
const char *firmwareVersion = "0.1.1";

unsigned long connectionInterval = 0;
unsigned long previousMillis = 0;

const char *updateBinaryUrl = "https://example.server.com/firmware.bin"; // must include either http:// or https://
const char *versionFileUrl = "https://example.server.com/firmware.txt";

WiFi_FirmwareUpdater update;
HTTPClient client;


void setup()
{
  update.configure(ssid, wifiPassword, firmwareVersion);
  Serial.begin(BAUD_RATE);
}

void loop()
{
  if (millis() - previousMillis >= connectionInterval) {
    previousMillis = millis();

    if (update.checkUpdateAvailable(client, versionFileUrl)) {
      update.updateFirmware(client, updateBinaryUrl);
    }
  }
}
