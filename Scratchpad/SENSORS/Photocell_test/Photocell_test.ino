/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>

File myFile;

const int pho1 = A0;
const int pho2 = A1;
const int pho3 = A2;
const int pho4 = A3;

int phoval1;
int phoval2;
int phoval3;
int phoval4;

void setup()
{
    pinMode(10, OUTPUT);
    pinMode(13, OUTPUT);
  
    Serial.begin(9600);
  
    if (!SD.begin(4)) {
      ledBlink(13, 7, 100, 100, 100, 100);
      return;
    }
    
    ledBlink(13, 2, 100, 100, 100, 100);
  
    myFile = SD.open("parsense.csv", FILE_WRITE);
    
    if (myFile)
    {
        myFile.println("cell1,cell2,cell3,cell4");
        myFile.close();
        ledBlink(13, 3, 100, 100, 100, 100);
    }
    
}

void loop()
{
    phoval1 = 1023 - analogRead(pho1);
    phoval2 = 1023 - analogRead(pho2);
    phoval3 = 1023 - analogRead(pho3);
    phoval4 = 1023 - analogRead(pho4);
    
    myFile = SD.open("parsense.csv", FILE_WRITE);
    
    if (myFile)
    {
        myFile.print(phoval1);
        myFile.print(",");
        myFile.print(phoval2);
        myFile.print(",");
        myFile.print(phoval3);
        myFile.print(",");
        myFile.print(phoval4);
        myFile.println();
        myFile.close();
        ledBlink(13, 4, 100, 100, 100, 100);
    }
    

    
    delay(1000);
}

void ledBlink(int ledPinLocation, int flashes, int evenOn, int evenOff, int oddOn, int oddOff)
{
    // Utility function for blinking an LED.
    // It intentionally blocks with delay() to buy time for processes.
    bool even = false;
    for(int i=0; i<flashes; i++)
    {
        digitalWrite(ledPinLocation, HIGH);
        
        if (even == false)
            delay(evenOn);
        else
            delay(oddOn);
            
        digitalWrite(ledPinLocation, LOW);
        
        if (even == false)
            delay(evenOff);
        else
            delay(oddOff);
            
        even = !even;
    }
}
