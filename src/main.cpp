#include "config.h"
#include <WiFi_FirmwareUpdater.h>

// Updates
WiFi_FirmwareUpdater update(SSID, PASSWORD, CURRENT_VERSION);


void setup()
{
  Serial.begin(BAUD_RATE);
  delay(1000);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_WARN);

  Serial.printf("[Current Firmware Version] %s\n", CURRENT_VERSION);
}

void loop()
{
  Serial.println("[Checking firmware versions]");

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
