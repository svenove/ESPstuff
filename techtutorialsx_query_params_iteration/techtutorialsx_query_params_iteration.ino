// Example code from https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/

//


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP)



void handleGenericArgs() { //Handler

  String message = "Number of args received: ";
  message += server.args();            //Get number of parameters
  message += "\n";                            //Add a new line

  for (int i = 0; i < server.args(); i++) {

    message += “Arg nº” + (String)i + " – > ";  //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter

  }

  server.send(200, "text / plain", message);     //Response to the HTTP request

}



void handleSpecificArg() {

  String message = "";

  if (server.arg("Temperature") == “”) {   //Parameter not found

    message = "Temperature Argument not found";

  } else {    //Parameter found

    message = "Temperature Argument = ";
    message += server.arg("Temperature");     //Gets the value of the query parameter

  }

  server.send(200, "text / plain", message);        //Returns the HTTP response

}


void setup() {

  Serial.begin(115200);
  WiFi.begin("YourSSID", "YourPassword"); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection

    delay(500);
    Serial.println("Waiting to connect…");

  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP to access the server

  server.on(" / genericArgs", handleGenericArgs); //Associate the handler function to the path
  server.on(" / specificArgs", handleSpecificArg); //Associate the handler function to the path

  server.begin();                                       //Start the server
  Serial.println("Server listening");

}

void loop() {

  server.handleClient();    //Handling of incoming requests

}

