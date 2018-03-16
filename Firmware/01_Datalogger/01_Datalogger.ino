// #### Includes #################################################

#include <Wire.h>
#include <RTClib.h>
#include <LowPower.h>
#include <SPI.h>
#include <SD.h>
#include <Streaming.h>
// Just so that I have more data to log during testing.
#include <dht.h>
#define dht_dpin A0
dht DHT;

// #### Variable initialisation ##################################

RTC_DS1307 rtc;
const int reedPin = 2;
const int ledPin = 9;
File logFile;
File backupFile;
DateTime now;
int TipCounter = 0;
volatile bool tipEventDetected = false;
int lastHour = 0;
int lastDay = 0;
unsigned long timeOfLastTip = 0;

// #### Setup ####################################################

/*
    Error codes (Flashing built-in LED (usually pin 13):

    Fast-fast    RTC couldn't be initialised.
    Fast-slow    SD card couldn't be initialised.
    Slow-slow    Couldn't access DHT11.
    2 fast 5 Slow All OK, starting to log data.
*/

void setup ()
{
    pinMode(ledPin, OUTPUT);
    pinMode(reedPin, OUTPUT);
    digitalWrite(reedPin, HIGH); // When the switch closes, this pin gets pulled low.
    // For the Arduino SD library:
    pinMode(10, OUTPUT);
        
    // ---- RTC ------------------------------

    Wire.begin();
    rtc.begin();

    // Set the time by running the Set_RTC_Time sketch.

    if (! rtc.isrunning()) {
        while (true)
        {
            // Either the time is not set, or the RTC's oscillator is no good.
            // Stop running and just blink the LED like crazy.
            ledBlink(ledPin, 10, 100, 100, 100, 100);
        }
    }
    
    // ---- Timekeeping ----------------------

    now = rtc.now();

    lastHour = now.minute();
    lastDay = now.hour();
    timeOfLastTip = millis();
        
    // ---- SD card --------------------------

    if (!SD.begin(10))
    {
        while (true)
        {
            // Stop running and do short-long-short-long blinks.
            ledBlink(ledPin, 10, 100, 100, 500, 100);
        }
    }
    
     // ---- DHT 11 package -------------------

    DHT.read11(dht_dpin);
    if (DHT.humidity == 0)
    {
        while (true)
        {
            // Stop running and do slow long-long blinks.
            ledBlink(ledPin, 10, 750, 750, 750, 750);
        }
    }
    
    // ---- Test write -----------------------

    bool logExists = false;

    if (SD.exists("logfile.csv"))
        logExists = true;

    logFile = SD.open("logfile.csv", FILE_WRITE);

    if (logFile)
    {
        if (logExists == false)
            logFile << "Time,Tips,Hum%,TempC,Event,Volt" << endl; // Write a header row into a new file.
        
        logFile	<< datestamp(now) << "T" << timestamp(now) << "," << TipCounter << "," << DHT.humidity << "," << DHT.temperature << "," << "Rebooted" << "," << voltMeasure() << endl;
        // Need to be completely sure that writing is finished, so let's blink a little to pass time.
        ledBlink(ledPin, 2, 100, 50, 100, 50);
        logFile.close(); 
    }
    else
    {
        while (true)
        {
            // Stop running and do short-long-short-long blinks.
            ledBlink(ledPin, 10, 100, 100, 750, 100);
        }
    }
    
    // ---- Self-tests complete --------------

    // Give us some visual indication that everything is okay.
    ledBlink(ledPin, 5, 500, 100, 500, 100);
}

// #### Main loop ################################################

void loop ()
{  
    attachInterrupt(0, recordTip, LOW);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    
    // I was woken by either a bucket tip or the watchdog timer. Either way, I'm awake already,
    // so I may as well check the time.
    
    detachInterrupt(0);
    
    if (tipEventDetected == true)
    {
        ledBlink(ledPin, 3, 50, 50, 50, 50);
        // Oh, I got tipped while I was sleeping! Better write that down.
        TipCounter++;
        tipEventDetected = false;
    }
    
    now = rtc.now();
    
    if (now.minute() != lastHour)
    {
        // ledBlink(ledPin, 2, 50, 50, 50, 50);
        // It's a new hour! I can write data! \o/
        logWrite();
        TipCounter = 0;
        lastHour = now.minute();
    }
    
    if (now.hour() != lastDay)
    {
        logBackup();
        lastDay = now.hour();
        // It's a new day! I can back-up the log file, aww yiss! \o/
    }
}

// #### Function declarations ####################################

void recordTip()
{
    // I heard something. Probably a tip event!
    int timeDifference = int(abs(millis() - timeOfLastTip));
    
    if (timeDifference >= 2000)
    {
        tipEventDetected = true;
        timeOfLastTip = millis();
    }
}

void logWrite()
{
    DHT.read11(dht_dpin);
    
    float finalVolt = voltMeasure();
    
    logFile = SD.open("logfile.csv", FILE_WRITE);

    if (logFile)
    {
        logFile	<< datestamp(now) << "T" << timestamp(now) << "," << TipCounter << "," << DHT.humidity << "," << DHT.temperature << "," << "Normal" << "," << voltMeasure() << endl;
        // Need to be completely sure that writing is finished. Should take 0.25ms,
        // so here's me being 500% sure.
        delay(125);
        logFile.close();
    }
    else
    {
        // Couldn't open.
    }
}

void logBackup()
{
    String shortyear;
    shortyear += now.year();
    shortyear.remove(0,2); // Keep only the decade so that I have room for the hour.
    
    String backupFilename;
    backupFilename += shortyear;
    backupFilename += withLeadingZero(now.month());
    backupFilename += withLeadingZero(now.day());
    backupFilename += withLeadingZero(now.hour());
    backupFilename += ".csv";
    
    logFile = SD.open("logfile.csv", FILE_READ);
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
        // Couldn't open.
    }
    
    logFile = SD.open("logfile.csv", FILE_WRITE);

    if (logFile)
    {
        
        logFile	<< datestamp(now) << "T" << timestamp(now) << "," << TipCounter << "," << DHT.humidity << "," << DHT.temperature << "," << "Daily backup" << "," << voltMeasure() << endl;
        // Need to be completely sure that writing is finished. Should take 0.25ms,
        // so here's me being 500% sure.
        delay(125);
        logFile.close();
    }
    else
    {
        // Couldn't open.
    }
}

String datestamp(DateTime now)
{
    String output;
    output += now.year();
    output += "-";
    output += withLeadingZero(now.month());
    output += "-";
    output += withLeadingZero(now.day());
    return output;
}

String timestamp(DateTime now)
{
    String output;
    output += withLeadingZero(now.hour());
    output += ":";
    output += withLeadingZero(now.minute());
    output += ":";
    output += withLeadingZero(now.second());
    return output;
}

String withLeadingZero(int digits) {
    // Utility function for printing leading zeroes.
    String output;
    
    if (digits < 10)
        output += "0";
    
    output += digits;
    return output;
}

void ledBlink(int ledPin, int flashes, int evenOn, int evenOff, int oddOn, int oddOff)
{
    // Utility function for blinking an LED.
    // It intentionally blocks with delay() to buy time for processes.
    bool even = false;
    for(int i=0; i<flashes; i++)
    {
        digitalWrite(ledPin, HIGH);
        
        if (even == false)
            delay(evenOn);
        else
            delay(oddOn);
            
        digitalWrite(ledPin, LOW);
        
        if (even == false)
            delay(evenOff);
        else
            delay(oddOff);
            
        even = !even;
    }
}

float voltMeasure()
{
    // Voltage measurement code from W.A. Smith, http://startingelectronics.org
    
    int sum = 0;                    // sum of samples taken
    unsigned char sample_count = 0; // current sample number
    float voltage = 0.0;            // calculated voltage
    
    while (sample_count < 10)
    {
        sum += analogRead(A2);
        sample_count++;
        delay(10);
    }
    
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.015V is the calibrated reference voltage
    voltage = ((float)sum / (float)10 * 5.015) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value
    
    return voltage * 11.132;
}
