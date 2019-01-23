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
SoftwareSerial d(D5,D6);
int motorStatus=0;
int dhtHumidityValue;
int dhtTemperatureValue;
int soilMoistureValue;
int mq4Value;
int mq135Value;
int led=D5;
const char* ssid="AtmegaEvolution";
const char* password="12345678";
String getContentType(String filename);
void handleFileRead();
void handleNotFound();
void onMotor();
void runFan();
void runPump();
StaticJsonBuffer<1000> jsonBuffer1;
JsonObject& root1=jsonBuffer1.createObject();
  
void setup()
{
  pinMode(led,OUTPUT);
 Serial.begin(115200);
 s.begin(115200);//Baut rate for communication of nodeMCU and arduinoUNO
 while(!Serial)continue;
 Serial.print("Connecting to ssid please wait");
 wifiMulti.addAP("Sandy","12345678");
 wifiMulti.addAP("MSI","12345678");
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
  server.on("/",HTTP_GET,handleFileRead); // Call the 'handleFileRoot' function when a client requests URI "/"
  server.on("/LED", HTTP_POST,onMotor);
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
 if(d.available()>0)
  {
    root1.printTo(d);
  }  
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
  String path="/index.html";
  if (SPIFFS.exists(path)) { // If the file exists
 File file = SPIFFS.open(path, "r"); // Open it
 size_t sent = server.streamFile(file, "text/html"); // And send it to the client
 file.close(); // Then close the file again
}
Serial.println("\tFile Not Found");
}

void runFan()
{}
void runPump()
{}


void onMotor()
{
   root1["motorStatus"]=!motorStatus;
   handleFileRead();
}




void handleNotFound(){
server.send(404, "text/plain", "404: Not found");
}
