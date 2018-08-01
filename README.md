# ESPstuff
A repo to gather all the ESP-related stuff (sketches, schematics, etc).
The main goal is to create a sketch-template that can be used when making new IoT-sensorstuff, etc.

### Goals
- [ ] Implement [WifiManager](https://github.com/tzapu/WiFiManager)
- [ ] Add custom basic fields to WifiManager (MQTT-hostname, MQTT-port, TLS-fingerprint, user/pass, etc)
- [ ] Implement [DoubleResetDetector](https://github.com/datacute/DoubleResetDetector) for entering config-mode ([see example](https://github.com/kentaylor/WiFiManager/tree/master/examples/ConfigOnDoubleReset))
- [ ] Implement encrypted MQTT (based on [this](https://github.com/adafruit/Adafruit_MQTT_Library/blob/master/examples/adafruitio_secure_esp8266/adafruitio_secure_esp8266.ino) example)
- [ ] Implement support for DHT-sensors using [DHTesp](https://github.com/beegee-tokyo/DHTesp)

### Hardware
* NodeMCU v2
* DHT22 (AM2302)
* Case
