// General
#include <LowPower.h>

// For RTC
#include <Wire.h>
#include <DSRTCLib.h>

// For SD
#include <SPI.h>
#include <SD.h>

// For RTC
DS1339 RTC = DS1339(3, 1);

void setup() {
//    pinMode(8, INPUT_PULLUP);
//    pinMode(9, INPUT_PULLUP);
    pinMode(10, OUTPUT);
    Wire.begin();
    RTC.start();
    SD.begin(10);
    
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void loop() {
  // put your main code here, to run repeatedly:
}
