#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
//#include <WiFi.h>

String apiKey = "HDV2V19NF8YO45TW"; // Enter your Write API key from ThingSpeak
const char *ssid = "realme 3";     // replace with your wifi ssid and wpa2 key
const char *pass = "12345678";
const char* server = "api.thingspeak.com";

String openWeatherMapApiKey = "799b714c55a35b3d3fda245c9bdd6905";

String city = "Porto";
String countryCode = "PT";

unsigned long lastTime = 0;

unsigned long timerDelay = 10000;

const int SensorPin = A0;
int relaypin = 14;
String jsonBuffer;
 
WiFiClient client;
 
 
void setup() {
  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps
  pinMode(relaypin, OUTPUT);
  
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
    Serial.println("WiFi connected");
    delay(4000);
}  
 
 
void loop() 
{
  float soilmoisturepercent = 100-((analogRead(SensorPin)*100)/1023);

  Serial.println(soilmoisturepercent);
 
    
  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(soilmoisturepercent);
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
   
  }
    client.stop();
    if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      if((int)(myObject["main"]["humidity"])>80){
        if(soilmoisturepercent >=0 && soilmoisturepercent <= 30)
  {
    digitalWrite(relaypin, HIGH);
    Serial.println("Motor is ON");
  }
  else if (soilmoisturepercent >30 && soilmoisturepercent <= 100)
  {
    digitalWrite(relaypin, LOW);
    Serial.println("Motor is OFF");
  }

      }
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
