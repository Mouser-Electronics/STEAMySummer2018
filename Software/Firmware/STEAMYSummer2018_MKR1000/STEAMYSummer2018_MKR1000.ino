/*************************************************** 
  
  
  Released under BSD license, all text above must be included in any redistribution
  
  Thanks to Adafruit for the Si1145 library:
  This is a library for the Si1145 UV/IR/Visible Light Sensor

  Thanks to ROb Tillaart for running average algorithm theory

  Designed specifically to work with the Si1145 sensor in the
  adafruit shop
  ----> https://www.adafruit.com/products/1777
  (also http://www.mouser.com)

  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include "Adafruit_SI1145.h"
#include <WiFi101.h>
#include <WiFiSSLClient.h>
#include "Wifi_Info.h"

Adafruit_SI1145 uv = Adafruit_SI1145();
WiFiSSLClient client;

static const int DELAY_AMT = 60000;  //approx. one minute
static const int AVG_ARRAY_SIZE = 60;
static const int BUZZER_PIN = 6;
static const int TONE_FREQ = 2500;
static const int RESET_BUTTON_PIN = 7;
static const float UV_THRESHOLD = 3.0;

const char* host = "maker.ifttt.com";  



/************************************************
 * 
 * 
 * 
 ***********************************************/
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(RESET_BUTTON_PIN, INPUT);
  
  Serial.begin(9600);
  Serial.println(F("Mouser STEAMy Summer UV Index Monitor"));
  
  if (! uv.begin()) {
    Serial.println(F("WARNING! Could NOT detect the Si1145 Light Sensor"));
    while (1);
  }


  Serial.println(F("Attempting to connect to WiFI Network..."));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //  Use WiFiClientSecure class to create TLS connection
  Serial.print(F("Connecting to "));
  Serial.println(host);
  if (!client.connect(host, 443)) {
    Serial.println(F("Connection Failed."));
    return;
  }
  Serial.println(F("You're connected to the network"));
  Serial.println(F("SYSTEM READY!"));
}


/************************************************
 * 
 * 
 * 
 ***********************************************/
void loop() {
  float currentUVindexReading = takeReading();
  float avgUVindexReading = runningAverage(currentUVindexReading);
  sendDataToCloud(currentUVindexReading);
  alertCheck(avgUVindexReading);
  delay(DELAY_AMT);
}



/************************************************
 * 
 * 
 * 
 ***********************************************/
float takeReading() {
  Serial.println(F("==================="));
  Serial.print(F("Vis: ")); Serial.println(uv.readVisible());
  Serial.print(F("IR: ")); Serial.println(uv.readIR());

  float UVindex = uv.readUV();
  UVindex /= 100.0;  // to get integer index, divide by 100
  Serial.print("UV: ");  Serial.println(UVindex);
  
  return UVindex;
}



/************************************************
 * 
 * 
 * 
 ***********************************************/
float runningAverage(float newReading) {
  static float AVG_UV_ARRAY[AVG_ARRAY_SIZE];
  static byte index = 0;
  static float sum = 0;
  static byte count = 0;

  sum -= AVG_UV_ARRAY[index];
  AVG_UV_ARRAY[index] = newReading;
  sum += AVG_UV_ARRAY[index];
  index++;
  index = index % AVG_ARRAY_SIZE;
  if (count < AVG_ARRAY_SIZE) count++;

  return sum / count;
}


/************************************************
 * 
 * 
 * 
 ***********************************************/
void alertCheck(float avgUVindexReading) {
  if (avgUVindexReading > UV_THRESHOLD ) {
    generateAlert();
  }
}


/************************************************
 * 
 * 
 * 
 ***********************************************/
void generateAlert() {
  tone(BUZZER_PIN, TONE_FREQ);
  while (digitalRead(RESET_BUTTON_PIN) == LOW){
  }

  noTone(BUZZER_PIN);
  
  //reset average for another hour
  for (int counter = 0; counter < 60; counter++) {
    runningAverage(0.0);
  }
}



/************************************************
 * Functio
 * 
 * 
 ***********************************************/
void sendDataToCloud(float currentUVindexReading) {

  String currentUVindexReadingString = String(currentUVindexReading);
  String  data = "{\"value1\":\"" + currentUVindexReadingString + "\"}";

  if (client.connect(host, 443)) {
    client.print("POST /trigger/Track_it/with/key/");
    client.print(IFTTT_APP_KEY);
    client.println(" HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);

    client.stop();
    
  }
  else {
    Serial.println("WARNING! CONNECTION TO CLOUD FAILED!");
  }

}

