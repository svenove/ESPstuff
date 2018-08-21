/*==============================================
================================================
*  Default setup();
*  Common setup is added here
*  It will call separate setup-methods for AP and STA-mode
*  Add spesific setup to setupAP() or setupSTA()
================================================
===============================================*/
void setup() {  
  Serial.begin(115200);
  while (!Serial) continue;
  
  initFS();
  
  // Prepare built-in LEDs 
  // (https://lowvoltage.github.io/2017/07/09/Onboard-LEDs-NodeMCU-Got-Two)
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D4, OUTPUT);

  // Double reset detected - AP-mode
  if (drd.detectDoubleReset()) {
    Serial.println("Double Reset Detected, setting builtin LED to low");
    digitalWrite(LED_BUILTIN, LOW);
    STAmode = false;

    setupAP();
  }
    
    //  STA-mode
  else {
    Serial.println("Double Reset NOT Detected, setting builtin LED to high");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(D4, HIGH);
    
    // Configure WIFI STA
    setupSTA();
  } 
} // end setup


/***********************************************
*  Setup-spesific for STA-mode
***********************************************/
void setupSTA() {
  Serial.println("Connecting to WIFI-network");
  char charSSID[ssid.length()+1];
  ssid.toCharArray(charSSID, ssid.length()+1);
  
  char charPwd[pwd_Wifipass.length()+1];
  pwd_Wifipass.toCharArray(charPwd, pwd_Wifipass.length()+1);

  Serial.print("SSID: ");
  Serial.println(charSSID);

  // If static IP
  if (!chk_DHCP) {
    IPAddress ipObj;
    ipObj.fromString(ip);

    IPAddress subObj;
    subObj.fromString(submask);

    IPAddress gwObj;
    gwObj.fromString(gw);

    IPAddress dnsObj;
    dnsObj.fromString(dns);

    WiFi.config(ipObj, dnsObj, gwObj, subObj); 
  }
  
  WiFi.hostname(hostname);
  WiFi.begin(charSSID, charPwd); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    Serial.println("Waiting to connect…");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP to access the server

  if (chk_configInSTA) {
    server.on("/", httpHandleRoot);
    server.on("/savePOST", httpSavePOST); //Associate the handler function to the path
    server.onNotFound(httpHandleRoot);

    server.begin();                                       //Start the HTTP-server
    Serial.println("Server listening");
  }

  dht.setup(dhtpin, DHTesp::DHT22); // Connect DHT sensor
}


/***********************************************
*  Setup-spesific for AP-mode
***********************************************/
void setupAP() {
  Serial.print("Setting up soft-AP ... ");
  if(WiFi.softAP(apssid, apwifipass)) {
    Serial.println("AP-mode entered successfully");
    Serial.println(WiFi.softAPIP());

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    if (dnsServer.start(53, "*", WiFi.softAPIP())) {
      Serial.println("Captive portal started");
    }

    server.on("/", httpHandleRoot);
    server.on("/savePOST", httpSavePOST); //Associate the handler function to the path
    server.onNotFound(httpHandleRoot);

    server.begin();                                       //Start the HTTP-server
    Serial.println("Server listening");
  }
  else {
    Serial.println("Failed to set up wifi in AP-mode!");
  }
}

