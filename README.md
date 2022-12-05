<p align="center">
    <a href="https://opensource.org/licenses/GPL-3.0">
        <img src="https://img.shields.io/badge/License-GPL%203.0-green.svg" alt="License" \>
    </a>
    <img src="https://github.com/RobertByrnes/WiFi_FirmwareUpdater/actions/workflows/ci.yml/badge.svg" alt="PlatformIO CI" \>
    <img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/RobertByrnes/WiFi_FirmwareUpdater?color=purple" />
</p>

# Features

`WiFi_FirmwareUpdater` provides the following features:

- Simple high level API
- Test driven semantic version support

# Getting Started

You can get started with `WiFi_FirmwareUpdater` by following the examples in the examples directory.

The Arduino examples gives the simplest format for use, after configuring the library with a function call:

```
update.configure(ssid, wifiPassword, firmwareVersion);
```

Check for a version file on the remote server, compare with existing firmware version and update if a newer version is available with a short code block:

```
if (update.checkUpdateAvailable(client, versionFileUrl)) {
    update.updateFirmware(client, updateBinaryUrl);
}
```
- See the example for data types and constructing the update object.

# More advanced use

Follow the platformio-arduino example to enable multiple (currently up to 3) sets of WiFi credentials, this allows fail over credentials to be used:

```
WiFi_Credentials keys {
  ssid1,
  wifiPassword1,
  ssid2,
  wifiPassword2,
  ssid3,
  wifiPassword3
};
```

The following code block will connect to the first set of WiFi credentials found from the set given. If the version available on the remote is the same or lower the update is skipped. If it is higher the update will begin and the ESP.restart() method is called on completion.

```
if (update.checkUpdateAvailable(client, versionFileUrl)) {
    log_i("Connected at IP Address %s", update.ipAddress().toString());
    log_i("Available version %s", update.availableFirmwareVersion());
    update.updateFirmware(client, updateBinaryUrl);
} else {
    log_w(update.errorString());
}
```
# Todo
- Remove remaining references to `String` class
- Add check for `client` suitability
- Update ```const char * WiFi_FirmwareUpdater::errorString()```
