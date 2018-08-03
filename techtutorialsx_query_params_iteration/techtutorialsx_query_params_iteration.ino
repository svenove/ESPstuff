// Example code from https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
    AND
    bbx10/ESPWebForm.ino https://gist.github.com/bbx10/5a2885a700f30af75fc5 
//


#include <ESP8266WiFi.h> //??
#include <WiFiClient.h> //??
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP)
MDNSResponder mdns;


const char INDEX_HTML[] =
  "<!DOCTYPE HTML>"
  "<html>"
  "<head>"
  "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
  "<title>ESP8266 Web Form Demo</title>"
  "<style>"
  "\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
  "</style>"
  "</head>"
  "<body>"
  "<h1>ESP8266 Web Form Demo</h1>"
  "<FORM action=\"/\" method=\"post\">"
  "<P>"
  "LED<br>"
  "<INPUT type=\"radio\" name=\"LED\" value=\"1\">On<BR>"
  "<INPUT type=\"radio\" name=\"LED\" value=\"0\">Off<BR>"
  "<INPUT type=\"submit\" value=\"Send\"> <INPUT type=\"reset\">"
  "</P>"
  "</FORM>"
  "</body>"
  "</html>";


//  example send static HTML page
//  server.send(200, "text/html", INDEX_HTML);



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

// example connection close
void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
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

  if (mdns.begin("esp8266WebForm", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on(" / genericArgs", handleGenericArgs); //Associate the handler function to the path
  server.on(" / specificArgs", handleSpecificArg); //Associate the handler function to the path

  server.begin();                                       //Start the server
  Serial.println("Server listening");

}

void loop() {

  server.handleClient();    //Handling of incoming requests

}

