// ORIGIN https://github.com/datacute/OLED_MQTT/blob/master/OLED_MQTT.ino

#include <FS.h>
#include <DoubleResetDetector.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

DoubleResetDetector drd(10,0);
bool fsMounted = false;
void initFS()
{
  //read configuration from FS json
  Serial.println("mounting FS...");

  fsMounted = SPIFFS.begin();
  if (fsMounted) {
    Serial.println("Mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        Serial.println();
        if (json.success()) {
          Serial.println("parsed json");

          if (json.containsKey("mqtt_server")) strcpy(mqttServer, json["mqtt_server"]);
          if (json.containsKey("mqtt_port")) strcpy(mqttPort, json["mqtt_port"]);
          if (json.containsKey("subscription")) strcpy(subscription, json["subscription"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    } else {
      Serial.println("/config.json not found");
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  custom_mqtt_server = new WiFiManagerParameter("mqtt_server", "MQTT Server", mqttServer, 40);
  custom_mqtt_port = new WiFiManagerParameter("mqtt_port", "MQTT Port", mqttPort, 6);
  custom_subscription = new WiFiManagerParameter("subscription", "MQTT Subscription", subscription, 60);
}

//callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

//callback notifying us of the need to save config
void saveConfig() {
  if (fsMounted) {

    Serial.println("Saving config");
  
    //read updated parameters
    strcpy(mqttServer, custom_mqtt_server->getValue());
    strcpy(mqttPort, custom_mqtt_port->getValue());
    strcpy(subscription, custom_subscription->getValue());
  
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqttServer;
    json["mqtt_port"] = mqttPort;
    json["subscription"] = subscription;
  
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    } else {
      json.printTo(Serial);
      Serial.println();
      json.printTo(configFile);
      configFile.close();
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end save
  shouldSaveConfig = false;
  drd.stop();
}

void wifiBegin() {
  //reset settings - for testing
  //wifiManager.resetSettings();
  WiFi.printDiag(Serial); //Remove this line if you do not want to see WiFi password printed
  bool doubleReset = drd.detectDoubleReset();
  bool noSSID = WiFi.SSID()=="";
  if (doubleReset || noSSID) {
    oled.clear();
    oled.println("Configure");
    if (noSSID) {
      oled.println("(No SSID)");
    }
    if (doubleReset) {
      oled.println("(2 x Reset)");
    }
    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);
  
    //add all your parameters here
    wifiManager.addParameter(custom_mqtt_server);
    wifiManager.addParameter(custom_mqtt_port);
    wifiManager.addParameter(custom_subscription);
  
    oled.println("Connect to");
    oled.println(configSSID);
    if (!wifiManager.startConfigPortal((char*) configSSID.c_str())) {
      Serial.println("failed to connect and should not get here");
    }
  
    //save the custom parameters to FS
    if (shouldSaveConfig) {
      saveConfig();
    }
    //rebooting is the easiest way to start making use of the new configuration.
    //once only wifi is being set, move this reboot to where the config setup failed.
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  else{
    WiFi.mode(WIFI_STA); // Force to station mode because if device was switched off while in access point mode it will start up next time in access point mode.
    unsigned long startedAt = millis();
    Serial.print("After waiting ");
    int connRes = WiFi.waitForConnectResult();
    float waited = (millis()- startedAt);
    Serial.print(waited/1000);
    Serial.print(" secs in setup() connection result is ");
    Serial.println(connRes);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  
      // Get ready for MQTT
      mqttClient.setServer(mqttServer, 1883);
      mqttClient.setCallback(callback);
    }
  }
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

int mqttState = 0;
unsigned long mqttStateChange = 0;

void mqttConnect() {
  Serial.print("Attempting MQTT connection to ");
  Serial.print(mqttServer);
  Serial.print(" as ");
  Serial.println(mqttClientName);

  oled.clear();
  oled.println("Connecting");
  oled.println(mqttServer);
  oled.println(mqttClientName);
  oled.println(subscription);
  
  // this is still blocking, when a solution is found, mqttState of 1 will mean connecting.
  if (mqttClient.connect((char*) mqttClientName.c_str())) {
    Serial.println("MQTT connected");
    mqttClient.subscribe(subscription);
    Serial.print("Subscribed to: ");
    Serial.println(subscription);
    oled.println("Subscribed");
    mqttState = 2;
    mqttStateChange = millis();
  } else {
    Serial.print("failed, rc=");
    Serial.println(mqttClient.state());
    oled.print("Failed ");
    oled.println(mqttClient.state());
    //Serial.println(" reset to try again");
    mqttState = 3;
    mqttStateChange = millis();
  }
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   initFS();
   wifiBegin();
}

void loop() {
  // put your main code here, to run repeatedly:
  drd.loop();
}
