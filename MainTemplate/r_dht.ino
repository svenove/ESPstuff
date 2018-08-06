/***********************************************
*  DHT
***********************************************/
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
