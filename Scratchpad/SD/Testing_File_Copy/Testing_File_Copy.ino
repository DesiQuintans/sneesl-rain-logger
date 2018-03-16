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

File logFile;
File backupFile;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
//  logFile = SD.open("test.txt", FILE_WRITE);
//
//  // if the file opened okay, write to it:
//  if (logFile) {
//    Serial.print("Writing to test.txt...");
//    logFile.println("testing 1, 2, 3.");
//    // close the file:
//    logFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }

  sdBackup();

  // re-open the file for reading:
  backupFile = SD.open("20150411.TXT");
  if (backupFile) {
    Serial.println("20150411.TXT:");

    // read from the file until there's nothing else in it:
    while (backupFile.available()) {
      Serial.write(backupFile.read());
    }
    // close the file:
    backupFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening 20150411.TXT");
  }
}

void loop()
{
  // nothing happens after setup
}

void sdBackup()
{
    String backupFilename;
    backupFilename += "2015";
    backupFilename += "04";
    backupFilename += "11";
    backupFilename += ".TXT";
    
    logFile = SD.open("TEST.TXT", FILE_READ);
    backupFile = SD.open(backupFilename.c_str(), FILE_WRITE); // c_str() because SD.open wants a char array.

    if (logFile && backupFile)
    {
        while (logFile.available()) {
          backupFile.write(logFile.read());
        }
        // Stick a delay in for good measure. No idea if this is long enough; each log line
        // is supposed to be ~25 bytes, so at a write rate of 4 bytes per ms, that's only 4kb
        // of data. But really, you'd expect the SD library to handle writing timing.
        delay(1000);
        logFile.close();
        backupFile.close();
    }
    else
    {
        // Log file and backup file could not be opened. :(
    }
}
