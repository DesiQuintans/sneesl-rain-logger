
// #### Includes #################################################

// General
#include <LowPower.h>

// For RTC
#include <Wire.h>
#include <DSRTCLib.h>

// For SD
#include <SPI.h>
#include <SD.h>

// For monitoring the environment of the enclosure. Heat (baking
// inside the enclosure) and moisture (condensation on the PCB
// surfaces) are big potential problems.

#include <dht.h>
#define dht_dpin A0
dht DHT;

// #### Variable initialisation ##################################

// This is the header row placed at the top of every log file.
const String headerRow = "Time,Tips,Hum%,TempC,mV,Event";

// For error lights
#define LEDPIN 9 // I can't use the built-in LED because it is involved in SPI comms with the SD shield.

// For RTC
DS1339 RTC = DS1339(3, 1);

// For SD
const int chipSelect = 10;
File logFile;
File backupFile;

// For rain collector
#define REEDPIN 2

// For internal logic
int TipCounter = 0;
volatile bool tipEventDetected = false;
int lastHour = 0;
int lastDay = 0;
unsigned long timeOfLastTip = 0;

// #### Setup ####################################################

/*
    Error codes (Flashing LED on pin 9):

    Fast x3      RTC couldn't be initialised.
    Fast-slow    SD card couldn't be initialised.
    Slow-slow    Couldn't access DHT11.
    2 fast 5 Slow All OK, starting to log data.
*/

void setup ()
{
    // ---- Error LED ------------------------
    
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, LOW);

    // ---- Rain collector -------------------
    
    pinMode(REEDPIN, OUTPUT);
    digitalWrite(REEDPIN, HIGH); // When the switch closes, this pin gets pulled low.
    
    // ---- SD Card --------------------------
    
    pinMode(10, OUTPUT);
        
    // ---- RTC ------------------------------

    Wire.begin();
    RTC.start();

    // Set the time by running the Set_TinyDuino_Time sketch.

    if (!RTC.time_is_set()) {
        while (true)
        {
            // Stop running and just blink the LED fast-fast-fast.
            ledBlink(LEDPIN, 3, 100, 100, 100, 100);
            delay(200);
        }
    }
    
    // ---- Timekeeping ----------------------

    RTC.readTime();

    lastHour = int(RTC.getMinutes()); // These two RTC functions return unsigned chars :S
    lastDay = int(RTC.getHours());
    timeOfLastTip = millis();
        
    // ---- SD card --------------------------

    if (!SD.begin(10))
    {
        while (true)
        {
            // Stop running and do short-long-short-long blinks.
            ledBlink(LEDPIN, 10, 100, 100, 500, 100);
        }
    }
    
     // ---- DHT 11 package -------------------

    delay(1000);
    DHT.read11(dht_dpin);
    if (DHT.humidity == 0)
    {
        while (true)
        {
            // Stop running and do slow long-long blinks.
            ledBlink(LEDPIN, 10, 750, 750, 750, 750);
        }
    }
    
    // ---- Test write -----------------------

    bool logExists = false;

    if (hasEnoughPower())
    {
        if (SD.exists("logfile.csv"))
            logExists = true;
    
        logFile = SD.open("logfile.csv", FILE_WRITE);
    
        if (logFile)
        {
            if (logExists == false)
            {
                ledBlink(LEDPIN, 1, 100, 50, 100, 50);
                logFile.println(headerRow); // Write a header row into a new file.
            }
            
            logFile.println(buildOutput("Rebooted"));
            // Need to be completely sure that writing is finished, so let's blink a little to pass time.
            ledBlink(LEDPIN, 2, 100, 50, 100, 50);
            logFile.close();
        }
        else
        {
            while (true)
            {
                // Stop running and do short-long-short-long blinks.
                ledBlink(LEDPIN, 10, 100, 100, 750, 100);
            }
        }
    }
    
    // ---- Self-tests complete --------------

    // Give us some visual indication that everything is okay.
    ledBlink(LEDPIN, 5, 500, 100, 500, 100);
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
        ledBlink(LEDPIN, 3, 50, 50, 50, 50);
        // Oh, I got tipped while I was sleeping! Better write that down.
        TipCounter++;
        tipEventDetected = false;
    }
    
    RTC.readTime();
    
    if (int(RTC.getMinutes()) != lastHour)
    {
        // ledBlink(LEDPIN, 2, 50, 50, 50, 50);
        // It's a new hour! I can write data! \o/
        logWrite("Normal");
        TipCounter = 0;
        lastHour = int(RTC.getMinutes());
    }
    
    if (int(RTC.getHours()) != lastDay)
    {
        logBackup();
        lastDay = int(RTC.getHours());
        // It's a new day! I can back-up the log file, aww yiss! \o/
    }
}

// #### Function declarations ####################################

// ---- Utility functions ----------------
// I've gotta organise myself with a TOC because Arduino IDE doesn't have code folding! It's 2015, man!
//    1. void    recordTip()
//    2. String  buildOutput(String eventType)
//    3. String  iso8601DateTime()
//    4. String  withLeadingZero(int digits)
//    5. bool    hasEnoughPower()
//    6. void    ledBlink(int ledPinLocation, int flashes, int evenOn, int evenOff, int oddOn, int oddOff)
//    7. long    readVcc()
//    8. float   voltMeasure()

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

String buildOutput(String eventType)
{
    DHT.read11(dht_dpin);
    
    String output;
    output+= iso8601DateTime();
    output+= ",";
    output+= TipCounter;
    output+= ",";
    output+= DHT.humidity;
    output+= ",";
    output+= DHT.temperature;
    output+= ",";
    output+= readVcc();
    output+= ",";
    output+= eventType;
    return output;
}

String iso8601DateTime()
{
    String output;
    output += int(RTC.getYears());
    output += "-";
    output += withLeadingZero(int(RTC.getMonths()));
    output += "-";
    output += withLeadingZero(int(RTC.getDays()));
    output += "T";
    output += withLeadingZero(int(RTC.getHours()));
    output += ":";
    output += withLeadingZero(int(RTC.getMinutes()));
    output += ":";
    output += withLeadingZero(int(RTC.getSeconds()));
    return output;
}

String withLeadingZero(int digits)
{
    // Utility function for printing leading zeroes.
    String output;
    
    if (digits < 10)
        output += "0";
    
    output += digits;
    return output;
}

bool hasEnoughPower()
{
    // This function checks whether the available power is at least 2.8 V, which is what the TinyDuino's microSD shield needs to safely operate.
    // This should get called as a condition for every SD card access.
    
    if (int(readVcc()) >= 2800)
        return true;
    else
        return false;
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

long readVcc()
{
    // Read 1.1V reference against AVcc
    // set the reference to Vcc and the measurement to the internal 1.1V reference
    // http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
    
    #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ADMUX = _BV(MUX5) | _BV(MUX0);
    #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = _BV(MUX3) | _BV(MUX2);
    #else
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #endif  
    
    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Start conversion
    while (bit_is_set(ADCSRA,ADSC)); // measuring
    
    uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
    uint8_t high = ADCH; // unlocks both
    
    long result = (high<<8) | low;
    
    result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
    return result; // Vcc in millivolts
}

// ---- SD access functions --------------
// To be done whenever the card is accessed:
//    - Check that there is enough voltage to safely write to the card.
//    - Open whatever streams you need to.
//    - Build the output string.
//    - Write to the stream/s.
//    - Close everything down.

void logWrite(String eventType)
{
    if (hasEnoughPower())
    {
        logFile = SD.open("logfile.csv", FILE_WRITE);
    
        if (logFile)
        {
            logFile.println(buildOutput(eventType));
            // Need to be completely sure that writing is finished. Should take 0.25ms,
            // so here's me being super sure.
            delay(50);
            logFile.close();
        }
        else
        {
            // Couldn't open.
        }
    }
}

void logBackup()
{
    String shortyear;
    RTC.readTime();
    
    shortyear += int(RTC.getYears());
    shortyear.remove(0,2); // Keep only the decade so that I have room for the hour.
    
    String backupFilename;
    backupFilename += shortyear;
    backupFilename += withLeadingZero(int(RTC.getMonths()));
    backupFilename += withLeadingZero(int(RTC.getDays()));
    backupFilename += withLeadingZero(int(RTC.getHours()));
    backupFilename += ".csv";
    
    if (hasEnoughPower())
    {
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
    }
    
    logWrite("Daily backup");
}
