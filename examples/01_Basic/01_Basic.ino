/* **************************************************
*
*   Example Code for running ScioSense ENS190 on UART
*       tested with Arduino UNO and ESP32
*
*  **************************************************
*/

#include <Arduino.h>

#include <ens190.h>

#define rxPin 16
#define txPin 17

#define ENS190_DEFAULT_BAUD_RATE    9600
#define OUTPUT_BAUD_RATE            9600

ENS190 ens190;

// If your board only has one hardware serial bus, use SoftwareSerial
#ifndef ESP32
  #include <SoftwareSerial.h>
  SoftwareSerial softwareSerial = SoftwareSerial(rxPin, txPin);
#endif

void setup()
{
    Serial.begin(OUTPUT_BAUD_RATE);
    Serial.println("");

    // If your board supports a second hardware serial bus (like the ESP32)...
    #ifdef ESP32
      // ...then use Serial1 or Serial2
      Serial2.begin(ENS190_DEFAULT_BAUD_RATE, SERIAL_8N1, rxPin, txPin);
      ens190.begin(&Serial2);
    #else
      // ...otherwise go for SoftwareSerial
      softwareSerial.begin(ENS190_DEFAULT_BAUD_RATE);
      ens190.begin(&softwareSerial);
    #endif

    while (ens190.init() == false)
    {
        Serial.println("Error -- The ENS190 is not connected.");
        delay(1000);
    }

    Serial.print("ENS190 Serial No.: ");
    Serial.print((unsigned long)ens190.serialNumber);

    Serial.print(", FW version: ");
    Serial.println((char*)ens190.fwVersion);
    
}

void loop()
{
    if (ens190.update() == RESULT_OK)
    {
        Serial.print("CO2: ");
        Serial.print(ens190.getCo2());
        Serial.println(" ppm");


        Serial.println("-----------------------");
    }
    else Serial.println("Result loop not ok");

    delay(4000);
}