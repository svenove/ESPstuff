# ESPstuff
A repo to gather all the ESP-related stuff (sketches, schematics, etc).
The main goal is to create a sketch-template that can be used when making new IoT-sensorstuff, etc.

### Goals
* Activate AP-mode for configuration by pressing a button on the device
* In config-mode, present a webinterface for config og WiFi, MQTT, etc
* In normal mode, don't expose webinterface
* If unable to connect to configured wifi, don't start AP-mode (still wait for button for activation)
* Support for MQTT over TLS and authentication

### Why
Unable to find any good libraries/frameworks that supports MQTT over TLS! 
Many of them also exposes the webinterface in normal mode, which is bad for security. Some also defaults to AP-mode if normal mode fails, exposing configuration mode.

### Reference: 
* https://lobradov.github.io/FOSS-Firmware-comparison-overview/#common-areas-for-improvement
* https://lobradov.github.io/Quick-look-at-home-iot-security/

### Tasks
- [x] Implement [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer)
- [ ] Implement [DoubleResetDetector](https://github.com/datacute/DoubleResetDetector) for entering config-mode (see examples [here](https://github.com/kentaylor/WiFiManager/tree/master/examples/ConfigOnDoubleReset) and [here](https://github.com/witnessmenow/arduino-youtube-api/blob/master/examples/ESP8266/ChannelStatisticsWithWifiManagerAndDoubleReset/ChannelStatisticsWithWifiManagerAndDoubleReset.ino))
- [ ] Implement [ArduinoJSON](https://github.com/bblanchon/ArduinoJson) for saving/loading data
- [ ] Use the two onboard LEDs to indicate config-mode, connected to wifi, etc. See [example](https://lowvoltage.github.io/2017/07/09/Onboard-LEDs-NodeMCU-Got-Two)
- [ ] Implement encrypted MQTT (based on [this](https://github.com/adafruit/Adafruit_MQTT_Library/blob/master/examples/adafruitio_secure_esp8266/adafruitio_secure_esp8266.ino) example)
- [x] Implement support for DHT-sensors using [DHTesp](https://github.com/beegee-tokyo/DHTesp)

### Hardware
* NodeMCU v2 (LoLin)
