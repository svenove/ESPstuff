
/*==============================================
================================================
*  HTTP-server stuff
================================================
===============================================*/
/***********************************************
*  Show config-form on webserver-root (/)
***********************************************/
void httpHandleRoot() {
  Serial.println("Serving HTTP-root.");
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
  "function showStatic(){\r\n"
  "if (document.getElementById(\"staticcheck\").checked == true){\r\n"
    "document.getElementById('static').style.display ='none';\r\n"
  "} else {\r\n"
    "document.getElementById('static').style.display ='block';\r\n"
  "}\r\n"
"}\r\n"
  "function toggleShowPass(fieldID) {\r\n"
    "var x = document.getElementById(fieldID);\r\n"
    "if (x.type === \"password\") {\r\n"
    "    x.type = \"text\";\r\n"
    "} else {\r\n"
    "    x.type = \"password\";\r\n"
    "}\r\n"
  "}</script></head>"
"<body><center><h1>ESP8266 Config</h1></center>"
  "<div class=\"form-style-2\">"
    "<form action=\"/savePOST\" method=\"POST\">"
      "<span>Hostname <span class=\"required\">*</span></span>"
  "<input type=\"text\" class=\"input-field\" name=\"hostname\" value=\"\" required/><br />"
  "<span>WIFI SSID <span class=\"required\">*</span></span>"
  "<input type=\"text\" class=\"input-field\" name=\"ssid\" value=\"\" required /><br />"
  "<span>Password <span class=\"required\">*</span></span>"
  "<input type=\"password\" class=\"input-field\" name=\"pwd_Wifipass\" id=\"wifipass\" min=\"8\" value=\"\" required /><br />"
  "<input type=\"checkbox\" onclick=\"toggleShowPass('wifipass')\">Show Password<br />"
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
      "<span>MQTT password <span class=\"required\">*</span></span><input type=\"password\" class=\"input-field\" name=\"pwd_mqtt\" id=\"pwd_mqtt\" value=\"\" required/><br />"
      "<input type=\"checkbox\" onclick=\"toggleShowPass('pwd_mqtt')\">Show Password<br />"
      "<span>MQTT TLS-fingerprint <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_tls_fingerprint\" size=\"60\" placeholder=\"00:AA:11:BB:22:CC:33:DD\" value=\"\" required/><br />"
      "<hr><span>Config available in regular mode?</span><INPUT type=\"checkbox\" name=\"chk_configInSTA\" /><br /><br />"
      "<span>&nbsp;</span><input type=\"submit\" value=\"Send\" name=\"submit\" /> <INPUT type=\"reset\"><br /><br />"
    "</form><hr><span>MAC-address: " + WiFi.macAddress() + "</span><br /><span>Temperature: " + temperature + "</span></div></body></html>";

  server.sendHeader("Connection", "close");
  server.send(200, "text/html", INDEX_HTML);
}

/***********************************************
*  Save data posted from the config-form.
*  Config-form is available in AP-mode.
***********************************************/
void httpSavePOST() { //Handler
  Serial.println("Saving HTTP-form.");
  String message = "Number of args received: ";
  message += server.args();            //Get number of parameters
  message += "\n";                            //Add a new line

  for (int i = 0; i < server.args(); i++) {
    // Skip the submit-button...
    if (server.argName(i) == "submit") {
      continue;
    }
    // Change checkbox to a "boolean"
    else if (server.argName(i).startsWith("chk_")) {
      if (server.arg(i) == "on") {
        server.arg(i) = "true";
      }
      else {
        server.arg(i) = "false";
      }
    }
    // If a passwordfield is empty, don't save it
    else if (server.argName(i).startsWith("pwd_")) {
      if (server.arg(i) == "") {
        continue;
      }
    }
    message += "Arg no " + (String)i + " : ";  //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter
  }

  // For final version, where we present the correct root-site
  //server.send(200, "text / plain", INDEX_HTML);     //Response to the HTTP request
  server.send(200, "text/plain", message);     //Response to the HTTP request
}


void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// connection close
// <<<<< Currently not in use, do we need it? >>>>>>
void httpReturnOK() {
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "OK\r\n");
}
