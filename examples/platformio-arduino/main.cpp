#include <Arduino.h>
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

// Updates
WiFi_Credentials credentials {
  ssid1,
  wifiPassword2,
  ssid2,
  wifiPassword2,
  ssid3,
  wifiPassword3
};

WiFi_FirmwareUpdater update(credentials, firmwareVersion);

void setup()
{
  Serial.begin(BAUD_RATE);
  delay(1000);
  Serial.printf("[Current Firmware Version] %s\n", firmwareVersion);
}

void loop()
{
  if (update.checkUpdateAvailable(versionFileUrl)) {
    Serial.print("[Connected at IP Address] ");
    Serial.println(update.ipAddress());
    delay(3);
    Serial.print("[Firmware version available] ");
    Serial.println(update.availableFirmwareVersion());

    update.updateFirmware(updateBinaryUrl);
  } else {
    Serial.println(update.errorString());
  }
  
  while (true) { delay(1); }
}
