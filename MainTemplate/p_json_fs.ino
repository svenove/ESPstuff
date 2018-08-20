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

          if (json.containsKey("chk_DHCP")) chk_DHCP = json["chk_DHCP"].as<bool>();
          if (json.containsKey("ip")) ip = json["ip"].as<String>();
          if (json.containsKey("submask")) submask = json["submask"].as<String>();
          if (json.containsKey("gw")) gw = json["gw"].as<String>();
          if (json.containsKey("dns")) dns = json["dns"].as<String>();

          if (json.containsKey("mqtt_server")) mqtt_server = json["mqtt_server"].as<String>();
          if (json.containsKey("mqtt_port")) mqtt_port = json["mqtt_port"].as<int>();
          if (json.containsKey("mqtt_user")) mqtt_user = json["mqtt_user"].as<String>();
          if (json.containsKey("pwd_mqtt")) pwd_mqtt = json["pwd_mqtt"].as<String>();
          if (json.containsKey("mqtt_tls_fingerprint")) mqtt_tls_fingerprint = json["mqtt_tls_fingerprint"].as<String>();

          if (json.containsKey("chk_configInSTAP")) chk_configInSTA = json["chk_configInSTA"].as<bool>();
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
  
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["hostname"] = hostname;
    json["ssid"] = ssid;
    json["pwd_Wifipass"] = pwd_Wifipass;

    json["chk_DHCP"] = chk_DHCP;
    json["ip"] = ip;
    json["submask"] = submask;
    json["gw"] = gw;
    json["dns"] = dns;
    
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_user"] = mqtt_user;
    json["pwd_mqtt"] = pwd_mqtt;
    json["mqtt_tls_fingerprint"] = mqtt_tls_fingerprint;

    json["chk_configInSTA"] = chk_configInSTA;
  
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
