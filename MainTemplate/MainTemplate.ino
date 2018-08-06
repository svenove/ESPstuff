//      ESPstuff NodeMCU

// Example code from https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
//    AND
//    bbx10/ESPWebForm.ino https://gist.github.com/bbx10/5a2885a700f30af75fc5 



/***********************************************
*  Defaults in AP-mode
*    IP    192.168.4.1
*    SSID  ESPstuff_01
*    PASS  keepITsecret
***********************************************/

  
#include <DoubleResetDetector.h>  // https://github.com/datacute/DoubleResetDetector
#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <ESP8266WebServer.h>     // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
#include <ESP8266mDNS.h>          // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include <FS.h>                   // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/FS.h
#include <DHTesp.h>               // https://github.com/beegee-tokyo/DHTesp


/***********************************************
*  Double Reset detector
***********************************************/
// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);


/***********************************************
*  Webserver and DNS
***********************************************/
ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP)
MDNSResponder mdns;


/***********************************************
*  DHT
***********************************************/
DHTesp dht;
unsigned long previousMillis = 0;
float humidity = 0;
float temperature = 0;
const int dhtpin = 1;


/***********************************************
*  Wifi Mode
***********************************************/
//    WIFI mode
bool STAmode = true;
// AP mode default values
const char *apssid = "ESPstuff_01";        
const char *apwifipass = "keepITsecret";
String ssid = "";
String wifipass = "";


/***********************************************
*  MQTT
***********************************************/
char *mqttServer = "";      
char *mqttPort = "";
char *subscription = "";
String mqttClientName = "mini-display-"; // Should be hostname??


bool shouldSaveConfig = false;
