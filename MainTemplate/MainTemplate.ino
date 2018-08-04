// Example code from https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
//    AND
//    bbx10/ESPWebForm.ino https://gist.github.com/bbx10/5a2885a700f30af75fc5 

#include <ESP8266WiFi.h> //??
#include <WiFiClient.h> //??
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP)
MDNSResponder mdns;

String macAdr = WiFi.macAddress();

// Server root site
const char INDEX_HTML[] = 
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
  "function show1(){"
  "// Get the checkbox"
  "var checkBox = document.getElementById(\"staticcheck\");"
  "if (checkBox.checked == true){"
    "document.getElementById('static').style.display ='none';"
  "} else {"
    "document.getElementById('static').style.display ='block';"
  "}"
"}"
  "</script></head>"
"<body><center><h1>ESP8266 Config</h1></center>"
  "<div class=\"form-style-2\">"
    "<form action=\"/genericArgs\" method=\"POST\">"
      "<span>Hostname <span class=\"required\">*</span></span>"
  "<input type=\"text\" class=\"input-field\" name=\"hostname\" value=\"\" required/><br />"
      "<span>DHCP <span class=\"required\">*</span></span><INPUT type=\"checkbox\" id=\"staticcheck\" name=\"DHCP\" value=\"true\" onclick='show1();' checked ><br />"
        "<div id=\"static\">"
        "<span>IP address <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"ip\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
        "<span>Netmask <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"submask\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
        "<span>Gateway <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"gw\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
         "<span>DNS <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"dns\" value=\"\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
          "</div><hr>"
      "<span>MQTT server <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_server\" value=\"\" required/><br />"
      "<span>MQTT port <span class=\"required\">*</span></span><input type=\"tel\" min=\"1\" max=\"65535\" class=\"input-field\" name=\"mqtt_port\" value=\"\" required placeholder=\"8883\"/><br />"
      "<span>MQTT username <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_user\" value=\"\" required/><br />"
      "<span>MQTT password <span class=\"required\">*</span></span><input type=\"password\" class=\"input-field\" name=\"mqtt_pass\" value=\"\" required/><br />"
      "<span>MQTT TLS-fingerprint <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_tls_fingerprint\" placeholder:\"00:AA:11:BB:22:CC:33:DD\" value=\"\" required/><br />"
      "<hr><span>Config available in regular mode?</span><INPUT type=\"checkbox\" name=\"configInSTA\" value=\"false\" /><br /><br />"
      "<span>&nbsp;</span><input type=\"submit\" value=\"Send\" name=\"submit\" /> <INPUT type=\"reset\"><br /><br />"
    "</form><hr><span>MAC-address: macAdr </span></div></body></html>";

bool STAmode = true;

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
  server.send(200, "text/html", INDEX_HTML);
}



void setup() {  
  Serial.begin(115200);
  if (STAmode) {
    setupSTA();
  }
  else {
    setupAP();
  }  
}

void setupSTA() {
  WiFi.begin("SSID", "password"); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    Serial.println("Waiting to connect…");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP to access the server

  if (mdns.begin("esp8266WebForm", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  //server.on("/specificArgs", handleSpecificArg); //Associate the handler function to the path
  server.begin();                                       //Start the server
  Serial.println("Server listening");

  server.on("/", handleRoot);
  server.on("/genericArgs", handleGenericArgs); //Associate the handler function to the path
}

void setupAP() {
  
}




void loop() {
  if (STAmode) {
    loopSTA();
  }
  else {
    loopAP();
  }
}
void loopSTA() {
  server.handleClient();    //Handling of incoming requests
}

void loopAP() {
  
}
