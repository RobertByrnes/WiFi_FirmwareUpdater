#include <WiFi_FirmwareUpdater.h>

#define BAUD_RATE 115200

const char *ssid = "Home WiFi";
const char *wifiPassword = "k54feSIWS93923092kzx";
const char *firmwareVersion = "0.1.1";

unsigned long connectionInterval = 0;
unsigned long previousMillis = 0;

const char *updateBinaryUrl = "https://example.server.com/firmware.bin"; // must include either http:// or https://
const char *versionFileUrl = "https://example.server.com/firmware.txt";
// Updates
WiFi_FirmwareUpdater update(ssid, wifiPassword, firmwareVersion);


void setup()
{
  Serial.begin(BAUD_RATE);
}

void loop()
{
  if (millis() - previousMillis >= connectionInterval) {
    previousMillis = millis();

    if (update.checkUpdateAvailable(versionFileUrl)) {
      update.updateFirmware(updateBinaryUrl);
    }
  }
}
