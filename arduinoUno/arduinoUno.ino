#include<LiquidCrystal.h>
#include<dht.h>
#include<ArduinoJson.h>
#include<SoftwareSerial.h>
SoftwareSerial s(5,6);//Establishing recive and transmittion ports Rx and Tx for writing to NodeMCU
SoftwareSerial d(5,6);//Establishing recive and transmittion ports Rx and Tx for reading from NodeMCU
LiquidCrystal myLCD(12,11,5,4,3,2);//Establishing connection to lcd
dht DHT;
int dhtPin=3;//Humidity and temperature port
int mq135Pin=A0;//Co2 sensor
int mq4Pin=A1;//Airquality sensor
int soilMoisturePin=5;//SoilMoisture pin
int soilMoistureValue;
int coolentFan=5;
int  ledPin=12;
void runFan();
void runPump();
void lcdDisplay();
int mq135Value;
StaticJsonBuffer<1000> jsonBuffer1;//Buffer for recieving data
JsonObject& root1=jsonBuffer1.parseObject(d);

void setup() {
  myLCD.begin(16,2);
  pinMode(ledPin,OUTPUT);
  pinMode(dhtPin,INPUT);
  pinMode(mq135Pin,INPUT);
  pinMode(mq4Pin,INPUT);
  pinMode(soilMoisturePin,INPUT);
  Serial.begin(9600);//Estblishing communication with the serial port
  s.begin(115200);
  d.begin(115200);
  DHT.read11(dhtPin);
   runFan();
   runPump();
   lcdDisplay();  
}

void loop() 
{
  int mq4Value=analogRead(mq4Pin);
   mq135Value=analogRead(mq135Pin);
  soilMoistureValue=analogRead(soilMoisturePin);
  soilMoistureValue=map(soilMoistureValue,550,0,0,100);
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root= jsonBuffer.createObject();   
  root["Humidity"] =DHT.humidity;
  root["Temperature"] =DHT.temperature;
  root["soilMoisture"]=soilMoistureValue;
  root["mq4"]=mq4Value;
  root["mq135"]=mq135Value; 
  if(s.available()>0)
  {
    root.printTo(s);
  }
  //Communication from nodeMCU
  if(root1 == JsonObject::invalid())
    return;
   root1.prettyPrintTo(Serial); 
}

void runFan()
{
 DHT.read11(dhtPin);
 int dhtHum=DHT.humidity;//reads the humidity from the surrounding;
 delay(100);
 int dhtTemp=DHT.temperature;//reads the temperature from the surrounding;
 delay(100);
 if(dhtTemp>=31)
 {
  analogWrite(coolentFan,1024);
 }
 else if(dhtTemp<=30&&dhtTemp>=27)
 {
   switch(dhtTemp)
   {
    case 30:analogWrite(coolentFan,1024);
           break;
    case 29:analogWrite(coolentFan,682);
           break;  
    case 28:analogWrite(coolentFan,342);
          break; 
    default:analogWrite(coolentFan,0);
          break;                 
    }
 }
 else
 {
  digitalWrite(ledPin,HIGH);
 }
 if(DHT.humidity<60)
 {
  analogWrite(ledPin,1024);
 }
 if(int fanStatus=root1["fanStatus"])
  {
   analogWrite(coolentFan,HIGH);
  }
}

void runPump()
{
  if(soilMoistureValue<30)
  {
   analogWrite(soilMoisturePin,HIGH);
  }
  if(int motorStatus=root1["motorStatus"])
  {
   analogWrite(soilMoisturePin,HIGH);
  }  
}
  
void lcdDisplay()
{
    myLCD.setCursor(0,0);
    myLCD.print("Hum:");
    myLCD.setCursor(5,0);
    myLCD.print(DHT.humidity);
    myLCD.setCursor(9,0);
    myLCD.print("tem:");
    myLCD.setCursor(12,0);
    myLCD.print(DHT.temperature);
    myLCD.setCursor(0,1);
    myLCD.print("soM:");
    myLCD.setCursor(5,0);
    myLCD.print(soilMoistureValue);
    myLCD.setCursor(9,0);
    myLCD.print("Co2:");
    myLCD.setCursor(12,0);
    myLCD.print(mq135Value);
    
    }
  
