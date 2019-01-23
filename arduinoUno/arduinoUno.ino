#include<LiquidCrystal.h>
#include<dht.h>
#include<ArduinoJson.h>
#include<SoftwareSerial.h>
SoftwareSerial s(5,6);//Establishing recive and transmittion ports Rx and Tx
LiquidCrystal myLCD(12,11,5,4,3,2);//Establishing connection to lcd
dht DHT;
int dhtPin=3;//Humidity and temperature port
int mq135Pin=A0;//Co2 sensor
int mq4Pin=A1;//Airquality sensor
int soilMoisturePin=5;//SoilMoisture pin
int soilMoistureValue;
int mq4Value;
int mq135Value;
void setup() {
  pinMode(dhtPin,INPUT);
  pinMode(mq135Pin,INPUT);
  pinMode(mq4Pin,INPUT);
  pinMode(soilMoisturePin,INPUT);
  Serial.begin(9600);//Estblishing communication with the serial port
  s.begin(115200);
  DHT.read11(dhtPin);
}

void loop() 
{
  mq4Value=analogRead(mq4Pin);
  mq135Value=analogRead(mq135Pin);
  soilMoistureValue=analogRead(soilMoisturePin);
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

}
