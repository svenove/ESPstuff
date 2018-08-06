//      ESPstuff NodeMCU

// Example code from https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
//    AND
//    bbx10/ESPWebForm.ino https://gist.github.com/bbx10/5a2885a700f30af75fc5 



// Defaults in AP-mode
  //IP    192.168.4.1
  //SSID  ESPstuff_01
  //PASS  keepITsecret

  
#include <DoubleResetDetector.h> // https://github.com/datacute/DoubleResetDetector
#include <ESP8266WiFi.h>
#include <WiFiClient.h> //??
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <DHTesp.h> //https://github.com/beegee-tokyo/DHTesp

//    Double Reset detector
// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);


//    Webserver and DNS
ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP)
MDNSResponder mdns;

//    DHT
DHTesp dht;
unsigned long previousMillis = 0;
float humidity = 0;
float temperature = 0;
const int dhtpin = 1;

//    WIFI mode
bool STAmode = true;
// AP mode default values
const char *apssid = "ESPstuff_01";        
const char *apwifipass = "keepITsecret";
String ssid = "";
String wifipass = "";

//    MQTT 
char *mqttServer = "";      
char *mqttPort = "";
char *subscription = "";
String mqttClientName = "mini-display-"; // Should be hostname??


//    LED PINS
const int PIN_LED = 2; // D4 on NodeMCU and WeMos. Controls the onboard LED

bool shouldSaveConfig = false;

//    Generic HTTP arg handler
void handleGenericArgs() { //Handler
  String message = "Number of args received: ";
  message += server.args();            //Get number of parameters
  message += "\n";                            //Add a new line

  for (int i = 0; i < server.args(); i++) {
    if (server.argName(i) == "submit") {
      continue;
    }
    message += "Arg no " + (String)i + " : ";  //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter
  }

  // For final version, where we present the correct root-site
  //server.send(200, "text / plain", INDEX_HTML);     //Response to the HTTP request
  server.send(200, "text/plain", message);     //Response to the HTTP request
}


// example connection close
void returnOK()
{
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "OK\r\n");
}

void handleRoot() {
  // Server root site
String INDEX_HTML = 
   "<!DOCTYPE HTML>"
"<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale = 1.0, user-scalable=0\">"
  "<title>ESP8266 Config</title>"
  "<style type=\"text/css\">"
    "body { background-color: #232B2B; color: #FFFFFF; }"
    ".form-style-2 { max-width: 500px; padding: 20px 12px 10px 20px; font: 13px Arial, Helvetica, sans-serif; }"
    ".form-style-2 label > span{ width: 100px; font-weight: bold; float: left; padding-top: 8px; padding-right: 5px; }"
    ".form-style-2 span.required{ color:red; }"
    "#static { display: none; }"
    "input[type=number]::-webkit-inner-spin-button, input[type=number]::-webkit-outer-spin-button { -webkit-appearance: none; margin: 0; }"
  "</style><script>"
  "function showStatic(){"
  "if (document.getElementById(\"staticcheck\").checked == true){"
    "document.getElementById('static').style.display ='none';"
  "} else {"
    "document.getElementById('static').style.display ='block';"
  "}"
  "function toggleShowPass(fieldID) {"
    "var x = document.getElementById(fieldID);"
    "if (x.type === \"password\") {"
    "    x.type = \"text\";"
    "} else {"
    "    x.type = \"password\";"
    "}"
"}"
"}</script></head>"
"<body><center><h1>ESP8266 Config</h1></center>"
  "<div class=\"form-style-2\">"
    "<form action=\"/genericArgs\" method=\"POST\">"
      "<span>Hostname <span class=\"required\">*</span></span>"
  "<input type=\"text\" class=\"input-field\" name=\"hostname\" value=\"\" required/><br />"
  "<span>WIFI SSID <span class=\"required\">*</span></span>"
  "<input type=\"text\" class=\"input-field\" name=\"ssid\" value=\"\" required /><br />"
  "<span>Password <span class=\"required\">*</span></span>"
  "<input type=\"password\" class=\"input-field\" name=\"password\" min=\"8\" value=\"\" required /><br />"
  "<input type=\"checkbox\" onclick=\"toggleShowPass('password')\">Show Password<br />"
      "<span>DHCP</span><INPUT type=\"checkbox\" id=\"staticcheck\" name=\"DHCP\" value=\"true\" onclick='showStatic();' checked ><br />"
        "<div id=\"static\">"
        "<span>IP address <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"ip\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
        "<span>Netmask <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"submask\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
        "<span>Gateway <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"gw\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
         "<span>DNS <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"dns\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
          "</div><hr>"
      "<span>MQTT server <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_server\" value=\"\" required/><br />"
      "<span>MQTT port <span class=\"required\">*</span></span><input type=\"number\" min=\"1\" max=\"65535\" class=\"input-field\" name=\"mqtt_port\" value=\"\" required placeholder=\"8883\"/><br />"
      "<span>MQTT username <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_user\" value=\"\" required/><br />"
      "<span>MQTT password <span class=\"required\">*</span></span><input type=\"password\" class=\"input-field\" name=\"mqtt_pass\" value=\"\" required/><br />"
      "<input type=\"checkbox\" onclick=\"toggleShowPass('mqtt_pass')\">Show Password<br />"
      "<span>MQTT TLS-fingerprint <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_tls_fingerprint\" size=\"60\" placeholder=\"00:AA:11:BB:22:CC:33:DD\" value=\"\" required/><br />"
      "<hr><span>Config available in regular mode?</span><INPUT type=\"checkbox\" name=\"configInSTA\" value=\"false\" /><br /><br />"
      "<span>&nbsp;</span><input type=\"submit\" value=\"Send\" name=\"submit\" /> <INPUT type=\"reset\"><br /><br />"
    "</form><hr><span>MAC-address: " + WiFi.macAddress() + "</span><span>Temperature: " + temperature + "</span></div></body></html>";
  
  server.send(200, "text/html", INDEX_HTML);
}



void getTempHum() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  
  //Library has function for delaying according to sensor sampling time
  //Not used, as this is blocking, instead of millis
  //delay(dht.getMinimumSamplingPeriod()
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 2000) { //every 2 seconds this is true and readings are updated

    // Read temperature and humidity
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    previousMillis = currentMillis;
  }
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  }
}


//    JSON and Filesystem
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
  
  //custom_mqtt_server = new WiFiManagerParameter("mqtt_server", "MQTT Server", mqttServer, 40);
  //custom_mqtt_port = new WiFiManagerParameter("mqtt_port", "MQTT Port", mqttPort, 6);
  //custom_subscription = new WiFiManagerParameter("subscription", "MQTT Subscription", subscription, 60);
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
    //strcpy(mqttServer, custom_mqtt_server->getValue());
    //strcpy(mqttPort, custom_mqtt_port->getValue());
    //strcpy(subscription, custom_subscription->getValue());
  
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

  
void setupSTA() {
  Serial.println("Connecting to WIFI-network");
  WiFi.begin("SSID", "password"); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    Serial.println("Waiting to connect…");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP to access the server

  if (mdns.begin("espSTUFF", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  dht.setup(dhtpin, DHTesp::DHT22); // Connect DHT sensor to GPIO 2
}


void setupAP() {
  Serial.print("Setting up soft-AP ... ");
  boolean result = WiFi.softAP(apssid, apwifipass);
  if(result == true) {
    Serial.println("AP-mode entered successfully");
  }
  else {
    Serial.println("Failed to set up wifi in AP-mode!");
  }

  server.begin();                                       //Start the server
  Serial.println("Server listening");

  server.on("/", handleRoot);
  server.on("/genericArgs", handleGenericArgs); //Associate the handler function to the path
}


void loopSTA() {
  //    TEMPERATURE AND HUMIDITY
  // Read DHT sensordata
  getTempHum();
  
  //Example show temp + hum
  Serial.println("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  
  
  delay(1000);
}

void loopAP() {
  server.handleClient();    //Handling of incoming requests
}


void setup() {  
  Serial.begin(115200);
  
  initFS();
  
  //  if STAmode true and double reset NOT detected, enter STAmode  
  if (STAmode && !drd.detectDoubleReset()) {

    Serial.println("Double Reset NOT Detected, setting builtin LED to high");
    digitalWrite(LED_BUILTIN, HIGH);
    
    // Configure WIFI STA
    setupSTA();
  }
    
    // Configure AP-mode
  else {
    if (drd.detectDoubleReset()) {
      Serial.println("Double Reset Detected");
      digitalWrite(LED_BUILTIN, LOW);
      STAmode = true;
    }
    
    setupAP();
    }
 
} // end setup


void loop() {

  //    Double reset detection enabled
  drd.loop();
    
  //    WIFI MODE
  if (STAmode) {
    loopSTA();
  }
  else {
    loopAP();
  }
  
} // end loop

