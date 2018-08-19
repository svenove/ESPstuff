/*==============================================
================================================
*  JSON and filesystem stuff
================================================
===============================================*/

/***********************************************
*  Read and load config from filsystem
***********************************************/
bool fsMounted = false;
void initFS() {
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

          if (json.containsKey("hostname")) hostname = json["hostname"].as<String>();
          if (json.containsKey("ssid")) ssid = json["ssid"].as<String>();
          if (json.containsKey("pwd_Wifipass")) pwd_Wifipass = json["pwd_Wifipass"].as<String>();

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
}


//callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


/***********************************************
*  Write and save config to filsystem
***********************************************/
void saveConfig() {
  if (fsMounted) {

    Serial.println("Saving config");
  
    //read updated parameters
    //strcpy(mqttServer, custom_mqtt_server->getValue());
    //strcpy(mqttPort, custom_mqtt_port->getValue());
    //strcpy(subscription, custom_subscription->getValue());
  
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["hostname"] = hostname;
    json["ssid"] = ssid;
    json["pwd_Wifipass"] = pwd_Wifipass;
  
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
