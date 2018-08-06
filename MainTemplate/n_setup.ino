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
  
  initFS();

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
    
    // Configure WIFI STA
    setupSTA();
    }
 
} // end setup


/***********************************************
*  Setup-spesific for STA-mode
***********************************************/
void setupSTA() {
  Serial.println("Connecting to WIFI-network");
  WiFi.begin("SSID", "password"); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    Serial.println("Waiting to connect…");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP to access the server

 // <<<<<<<<<<<<<< TESTING >>>>>>>>>>>>>>>>>
  server.on("/", httpHandleRoot);
  server.on("/savePOST", httpSavePOST); //Associate the handler function to the path
  server.onNotFound(handleNotFound);

  server.begin();                                       //Start the HTTP-server
  Serial.println("Server listening");
 // <<<<<<<<<<<<<< TESTING >>>>>>>>>>>>>>>>> 

  dht.setup(dhtpin, DHTesp::DHT22); // Connect DHT sensor
}


/***********************************************
*  Setup-spesific for AP-mode
***********************************************/
void setupAP() {
  Serial.print("Setting up soft-AP ... ");
  if(WiFi.softAP(apssid, apwifipass)) {
    Serial.println("AP-mode entered successfully");
    
    if (mdns.begin("espSTUFF", WiFi.localIP())) {
      Serial.println("MDNS responder started");
    }

    server.on("/", httpHandleRoot);
    server.on("/savePOST", httpSavePOST); //Associate the handler function to the path
    server.onNotFound(handleNotFound);

    server.begin();                                       //Start the HTTP-server
    Serial.println("Server listening");
  }
  else {
    Serial.println("Failed to set up wifi in AP-mode!");
  }
}

