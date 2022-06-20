#include "config.h"
#include <WiFi_FirmwareUpdater.h>

// Updates
WiFi_FirmwareUpdater update(SSID, PASSWORD, CURRENT_VERSION);


void setup()
{
  Serial.begin(BAUD_RATE);

  Serial.printf("[Current Firmware Version] %s\n", CURRENT_VERSION);

  Serial.println("[i] Checking firmware versions...");

  if (update.checkUpdateAvailable(UPDATE_VERSION_FILE_URL)) {
    
    Serial.print("[i] Firmware version available: ");
    Serial.println(update.getAvailableFirmwareVersion());

    update.updateFirmware(UPDATE_URL);
  }

}

void loop() { while (true) { delay(1); } }
