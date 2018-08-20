/*==============================================
================================================
*  Default loop();
*  Common loop-code is added here
*  It will call separate loop-methods for AP and STA-mode
*  Add spesific loop-code to loopAP() or loopTA()
================================================
===============================================*/
void loop() {    
  //    WIFI MODE
  if (STAmode) {
    loopSTA();
  }
  else {
    loopAP();
  }
} // end loop


/***********************************************
*  Loop-spesific for STA-mode
***********************************************/
void loopSTA() {
  // <<<<<<<<<<<<< TESTING >>>>>>>>>>>>
  server.handleClient();    //Handling of incoming requests to HTTP-server
  
   //    Double reset detection enabled
  drd.loop();
  
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


/***********************************************
*  Loop-spesific for AP-mode
***********************************************/
void loopAP() {
  dnsServer.processNextRequest();
  server.handleClient();    //Handling of incoming requests to HTTP-server
}
