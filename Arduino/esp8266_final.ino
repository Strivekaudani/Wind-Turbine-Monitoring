/*
 * HTTP Client POST Request
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */
const char *ssid = "Loading...";  //ENTER YOUR WIFI SETTINGS
const char *password = "shamiso246";

const char *host = "192.168.43.14";   

String data = "";
int counter = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void loop() {
  if(Serial.available()) 
  {
    HTTPClient http;    //Declare object of class HTTPClient
    WiFiClient doodah;

    String postData;
    data += (char)Serial.read();
    counter++;
    
//    Serial.write(Serial.read());
    delayMicroseconds(10);

    if(counter >= 4)
    {
      Serial.print(data);
      http.begin(doodah, "http://192.168.43.14:8090/post");              //Specify request destination
      http.addHeader("Content-Type", "text/plain");    //Specify content-type header
    
      int httpCode = http.POST(data);   //Send the request
      String payload = http.getString();    //Get the response payload
    
      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
    
      http.end();

      data = "";
      counter = 0;
    }
  }  //Post Data at every 5 seconds
}
//=============
