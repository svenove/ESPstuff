
/*==============================================
================================================
*  HTTP-server stuff
================================================
===============================================*/
/***********************************************
*  Show config-form on webserver-root (/)
***********************************************/
void httpHandleRoot() {
  initFS();
  
  Serial.println("Serving HTTP-root.");
  // Server root site
String INDEX_HTML = 
   "<!DOCTYPE HTML>"
"<html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale = 1.0, user-scalable=0\">"
  "<title>ESP8266 Config</title>"
  "<style>"
    "body { background-color: #232B2B; color: #FFFFFF; }"
    ".form-style-2 { max-width: 500px; padding: 20px 12px 10px 20px; font: 13px Arial, Helvetica, sans-serif; }"
    ".form-style-2 label > span{ width: 100px; font-weight: bold; float: left; padding-top: 8px; padding-right: 5px; }"
    ".form-style-2 span.required{ color:red; }"
    "#static { display: ";
if (chk_DHCP) {
  INDEX_HTML += "none";
}
else {
  INDEX_HTML += "block";
}
    INDEX_HTML += "; }"
    "input[type=number]::-webkit-inner-spin-button, input[type=number]::-webkit-outer-spin-button { -webkit-appearance: none; margin: 0; }"
  "</style><script>"
  "function showStatic(){\r\n"
  "if (document.getElementById(\"chk_DHCP\").checked == true){\r\n"
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
  "<input type=\"text\" class=\"input-field\" name=\"hostname\" value=\"" + hostname + "\" required/><br />"
  "<span>WIFI SSID <span class=\"required\">*</span></span>"
  "<input type=\"text\" class=\"input-field\" name=\"ssid\" value=\"" + ssid + "\" required /><br />"
  "<span>Password </span>"
  "<input type=\"password\" class=\"input-field\" name=\"pwd_Wifipass\" id=\"wifipass\" value=\"\" /><br />"
  "<input type=\"checkbox\" onclick=\"toggleShowPass('wifipass')\">Show Password<br />"
      "<span>DHCP</span>" + createCheckbox("chk_DHCP", chk_DHCP, "showStatic()") + "<br />"
        "<div id=\"static\">"
        "<span>IP address <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"ip\" value=\"" + ip + "\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
        "<span>Netmask <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"submask\" value=\"" + submask + "\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
        "<span>Gateway <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"gw\" value=\"" + gw + "\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
         "<span>DNS <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"dns\" value=\"" + dns + "\" pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\" /><br />"
          "</div><hr>"
      "<span>MQTT server <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_server\" value=\"" + mqtt_server + "\" required/><br />"
      "<span>MQTT port <span class=\"required\">*</span></span><input type=\"number\" min=\"1\" max=\"65535\" class=\"input-field\" name=\"mqtt_port\" value=\"" + mqtt_port + "\" required placeholder=\"8883\"/><br />"
      "<span>MQTT username <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_user\" value=\"" + mqtt_user + "\" required/><br />"
      "<span>MQTT password </span><input type=\"password\" class=\"input-field\" name=\"pwd_mqtt\" id=\"pwd_mqtt\" value=\"\" /><br />"
      "<input type=\"checkbox\" onclick=\"toggleShowPass('pwd_mqtt')\">Show Password<br />"
      "<span>MQTT TLS-fingerprint <span class=\"required\">*</span></span><input type=\"text\" class=\"input-field\" name=\"mqtt_tls_fingerprint\" size=\"60\" placeholder=\"00:AA:11:BB:22:CC:33:DD\" value=\"" + mqtt_tls_fingerprint + "\" required/><br />"
      "<hr>" + createCheckbox("chk_configInSTA", chk_configInSTA) + "<br /><span>Config available in regular mode</span><br /><br />"
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
    // If a passwordfield is empty, don't save it
    else if (server.argName(i).startsWith("pwd_")) {
      if (server.arg(i) == "") {
        continue;
      }
    }
    message += "Arg no " + (String)i + " : ";  //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter

    // A "checkbox" is only POSTed if checked. Initiate to "false" to handle that.
    chk_DHCP = false;
    chk_configInSTA = false;

    if (server.argName(i) == "hostname") {
      hostname = server.arg(i);
    }
    else if (server.argName(i) == "ssid") {
      ssid = server.arg(i);
    }
    else if (server.argName(i) == "pwd_Wifipass") {
      pwd_Wifipass = server.arg(i);
    }
    else if (server.argName(i) == "chk_DHCP") {
      chk_DHCP = true;
    }
    else if (server.argName(i) == "ip") {
      ip = server.arg(i);
    }
    else if (server.argName(i) == "submask") {
      submask = server.arg(i);
    }
    else if (server.argName(i) == "gw") {
      gw = server.arg(i);
    }
    else if (server.argName(i) == "dns") {
      dns = server.arg(i);
    }
    else if (server.argName(i) == "mqtt_server") {
      mqtt_server = server.arg(i);
    }
    else if (server.argName(i) == "mqtt_port") {
      mqtt_port = server.arg(i).toInt();
    }
    else if (server.argName(i) == "mqtt_user") {
      mqtt_user = server.arg(i);
    }
    else if (server.argName(i) == "pwd_mqtt") {
      pwd_mqtt = server.arg(i);
    }
    else if (server.argName(i) == "mqtt_tls_fingerprint") {
      mqtt_tls_fingerprint = server.arg(i);
    }
    else if (server.argName(i) == "chk_configInSTA") {
      chk_configInSTA = true;
    }
  }

  // Save to JSON-file
  //saveConfig();

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

String createCheckbox(String name, bool checked) {
  return createCheckbox(name, checked, "");
}

String createCheckbox(String name, bool checked, String onClick) {
  String html = "<INPUT type=\"checkbox\" id=\"" + name + "\" name=\"" + name + "\"";
  if (onClick != "") {
    html += " onclick='" + onClick + ";'";
  }
  if (checked) {
    html += " checked";
  }
  html += " />";

  return html;
}

