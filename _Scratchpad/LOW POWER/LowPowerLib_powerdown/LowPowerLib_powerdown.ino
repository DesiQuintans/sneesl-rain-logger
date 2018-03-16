// **** INCLUDES *****
#include "LowPower.h"
#include <JeeLib.h>


#include <Wire.h>
#include <DSRTCLib.h>
#include <SPI.h>
#include <SD.h>
DS1339 RTC = DS1339(3, 1);


void setup()
{
    pinMode(13, OUTPUT);


    pinMode(10, OUTPUT);
    Wire.begin();
    RTC.start();
    SD.begin(10);
    
    
}

void loop() 
{
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

    digitalWrite(13, HIGH);
    Sleepy::loseSomeTime(1000);
    digitalWrite(13, LOW);
}
