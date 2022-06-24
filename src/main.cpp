#include "config.h"
#include <WiFi_FirmwareUpdater.h>

// Updates
WiFi_Credentials credentials {
  SSID_1,
  PASSWORD_1,
  SSID_2,
  PASSWORD_2,
  SSID_3,
  PASSWORD_3
};

WiFi_FirmwareUpdater update(credentials, CURRENT_VERSION);

void setup()
{
  Serial.begin(BAUD_RATE);
  delay(1000);
  Serial.printf("[Current Firmware Version] %s\n", CURRENT_VERSION);
}

void loop()
{
  if (update.checkUpdateAvailable(UPDATE_VERSION_FILE_URL)) {
    Serial.print("[Connected at IP Address] ");
    Serial.println(update.ipAddress());
    delay(3);
    Serial.print("[Firmware version available] ");
    Serial.println(update.availableFirmwareVersion());

    update.updateFirmware(UPDATE_URL);
  } else {
    Serial.println(update.errorString());
  }
  
  while (true) { delay(1); }
}
