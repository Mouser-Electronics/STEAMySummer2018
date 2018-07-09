/*************************************************** 
  
  
  Released under BSD license, all text above must be included in any redistribution
  
  Thanks to Adafruit for the Si1145 library:
  This is a library for the Si1145 UV/IR/Visible Light Sensor

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

Adafruit_SI1145 uv = Adafruit_SI1145();

void setup() {
  Serial.begin(9600);
  
  Serial.println(F("Adafruit SI1145 test"));
  
  if (! uv.begin()) {
    Serial.println(F("Didn't find Si1145"));
    while (1);
  }

  Serial.println(F("OK!"));
}

void loop() {
  Serial.println(F("==================="));
  Serial.print(F("Vis: ")); Serial.println(uv.readVisible());
  Serial.print(F("IR: ")); Serial.println(uv.readIR());

  float UVindex = uv.readUV();
  // the index is multiplied by 100 so to get the
  // integer index, divide by 100!
  UVindex /= 100.0;  
  Serial.print("UV: ");  Serial.println(UVindex);

  delay(5000);
}
