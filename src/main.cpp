#include "config.h"
#include <WiFi_FirmwareUpdater.h>

// Updates
WiFi_FirmwareUpdater update(SSID, PASSWORD, CURRENT_VERSION);


void setup()
{
  Serial.begin(BAUD_RATE);
}

void loop()
{
  if (update.checkUpdateAvailable(UPDATE_VERSION_FILE_URL)) {
    update.updateFirmware(UPDATE_URL);
  }

}
