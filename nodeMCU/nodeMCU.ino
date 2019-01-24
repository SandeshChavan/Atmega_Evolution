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
int fanStatus=0;
int motorStatus=0;
int dhtHumidityValue;
int dhtTemperatureValue;
int soilMoistureValue;
int mq4Value;
int mq135Value;
int led=D5;

const char* ssid="AtmegaEvolution";
const char* password="12345678";
void handleFileRead();
void handleNotFound();
void onMotor();
void onFan();
void handleHum();
void handleTemp();
void handleSoil();
void handleAir();
void handleCO2();
void handleGraph();
StaticJsonBuffer<1000> jsonBuffer1;
JsonObject& root1=jsonBuffer1.createObject();

void setup()
{
   pinMode(led,OUTPUT);
  Serial.begin(115200);
  d.begin(115200);
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
   server.on("/onMotor", HTTP_POST,onMotor);
   server.on("/onFan", HTTP_POST,onFan);
   server.on("/getHum", HTTP_POST,handleHum);
   server.on("/getTemp", HTTP_POST,handleTemp);
   server.on("/getSoil", HTTP_POST,handleSoil);
   server.on("/getCO2", HTTP_POST,handleCO2);
   server.on("/getAir", HTTP_POST,handleAir);
   server.on("/plotGraph", HTTP_POST,handleGraph);
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
 //Writing to arduino
  if(d.available()>0)
  {
    root1.printTo(d);
  }  
   
  delay(100000); 
}


void handleFileRead()
{
  String path="/index.html";
  if (SPIFFS.exists(path)) { // If the file exists
 File file = SPIFFS.open(path, "r"); // Open it
 size_t sent = server.streamFile(file, "text/html"); // And send it to the client
 file.close(); // Then close the file again
}
Serial.println("\File Not Found");
}

void onMotor()
{
   root1["motorStatus"]=!motorStatus;
   handleFileRead();
}


void onFan()
{
   root1["fanStatus"]=!fanStatus;
   handleFileRead();
}

void handleHum()
{
 server.send(200, "text/plane", String(dhtHumidityValue)); 
  }
void handleTemp()
{
  server.send(200, "text/plane", String(dhtTemperatureValue));
}
void handleSoil()
{
  server.send(200, "text/plane", String(soilMoistureValue));
}
void handleAir()
{
  server.send(200, "text/plane", String(mq4Value));
}
void handleCO2()
{
  server.send(200, "text/plane", String(mq135Value));
}

void handleGraph()
{
  String path="/graph.html";
  if (SPIFFS.exists(path)) { // If the file exists
 File file = SPIFFS.open(path, "r"); // Open it
 size_t sent = server.streamFile(file, "text/html"); // And send it to the client
 file.close(); // Then close the file again

}
void handleNotFound(){
server.send(404, "text/plain", "404: Not found");
}
