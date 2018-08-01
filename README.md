# ESPstuff
A repo to gather all the ESP-related stuff (sketches, schematics, etc).
The main goal is to create a sketch-template that can be used when making new IoT-sensorstuff, etc.

### Goals
- [ ] Implement [WifiManager](https://github.com/tzapu/WiFiManager)
- [ ] Add custom basic fields to WifiManager (MQTT-hostname, MQTT-port, TLS-fingerprint, user/pass, etc)
- [ ] Implement [DoubleResetDetector](https://github.com/datacute/DoubleResetDetector) for entering config-mode (see examples [here](https://github.com/kentaylor/WiFiManager/tree/master/examples/ConfigOnDoubleReset) and [here](https://github.com/witnessmenow/arduino-youtube-api/blob/master/examples/ESP8266/ChannelStatisticsWithWifiManagerAndDoubleReset/ChannelStatisticsWithWifiManagerAndDoubleReset.ino))
- [ ] Use the two onboard LEDs to indicate config-mode, connected to wifi, etc. See [example](https://lowvoltage.github.io/2017/07/09/Onboard-LEDs-NodeMCU-Got-Two)
- [ ] Implement encrypted MQTT (based on [this](https://github.com/adafruit/Adafruit_MQTT_Library/blob/master/examples/adafruitio_secure_esp8266/adafruitio_secure_esp8266.ino) example)
- [ ] Implement support for DHT-sensors using [DHTesp](https://github.com/beegee-tokyo/DHTesp)

### Hardware
* NodeMCU v2
* DHT22 (AM2302)
* Case
