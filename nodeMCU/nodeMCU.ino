#include<ESP8266WiFi.h>//Wifi library
#include<ESP8266WiFiMulti.h>
#include<ESP8266WebServer.h>//Library for setting of WebServer
#include<ESP8266mDNS.h>multi//Library for multicast DNS
#include<WiFiClient.h>
#include<SoftwareSerial.h>
#include<FS.h>//Library for nodeMCU flash memory storage
#include<ArduinoJson.h>//Library for communication between NodeMCU and Arduino
ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);//Server port for communication
SoftwareSerial s(D6,D5);//Establishing recive and transmittion ports Rx and Tx
int dhtHumidityValue;
int dhtTemperatureValue;
int soilMoistureValue;
int mq4Value;
int mq135Value;
const char* ssid="AtmegaEvolution";
const char* password="12345678";
String getContentType(String filename);
void handleFileRead();
void handleNotFound();
void handleLED();
void runFan();
void runPump();
void setup()
{
 Serial.begin(115200);
 s.begin(115200);//Baut rate for communication of nodeMCU and arduinoUNO
 while(!Serial)continue;
 Serial.print("Connecting to ssid please wait");
 wifiMulti.addAP("Sandy","12345678");
 wifiMulti.addAP("Harshavardhan","12345678");
 while(wifiMulti.run()!=WL_CONNECTED)
 {
  delay(1000);
  Serial.print(".");
 }
 if(MDNS.begin("AtmegaEvolution"))
  {
    Serial.println("The mDNS is ready for use"); 
  }
 else
  {
    Serial.println("The mDNS is not ready for use"); 
  }
  SPIFFS.begin();
  server.onNotFound(handleNotFound);
  server.on("/", HTTP_GET,handleFileRead); // Call the 'handleFileRoot' function when a client requests URI "/"
  server.on("/LED", HTTP_POST, handleLED);
  server.begin();
  Serial.print(WiFi.localIP());
}

void loop()
{
  server.handleClient();
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(s);
  if(root == JsonObject::invalid())
    return; 
  root.prettyPrintTo(Serial); 
  dhtHumidityValue=root["Humidity"];
  dhtTemperatureValue=root["Temperature"];
  soilMoistureValue=root["soilMoisture"];
  mq4Value=root["mq4"];
  mq135Value=root["mq135"];  
  delay(100000); 
}

String getContentType(String filename) { // convert the file extension to the MIME type
if (filename.endsWith(".html")) return "text/html";
else if (filename.endsWith(".css")) return "text/css";
else if (filename.endsWith(".js")) return "application/javascript";
else if (filename.endsWith(".ico")) return "image/x-icon";
return "text/plain";
}

void handleFileRead()
{
  String path="index";
  File file = SPIFFS.open(path,"r");
  size_t sent = server.streamFile(file,"text/html"); // And send it to the client
  file.close(); // Then close the file again
}

void runFan()
{}
void runPump()
{}


void handleLED()
{ // If a POST request is made to URI /LED
  server.send(200, "text/html","<H1>Harsha u hopless fellow</H1>");
  server.sendHeader("Location","index.html"); // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303); // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}



void handleNotFound(){
server.send(404, "text/plain", "404: Not found");
}
