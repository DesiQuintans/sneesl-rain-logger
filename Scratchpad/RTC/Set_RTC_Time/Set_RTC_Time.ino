/*
    Sets RTC time to the compile time of this script, so compile it just before upload.
    It's off by a few seconds, but that's good enough for hourly monitoring.
    
    Pinout:
    +---------+--------+---------+
    |         | Module | Arduino |
    |---------|--------|---------|
    |         | Gnd    | GND     |
    |         | VCC    | 5V      |
    |         | SDA    | A4      |
    |         | SCL    | A5      |
    +---------+--------+---------+
*/

// #### Includes #################################################

#include <Wire.h>
#include <RTClib.h>
#include <Streaming.h>

// #### Variable initialisation ##################################

RTC_DS1307 rtc;
int ledPin = 13;
DateTime now;

// #### Setup ####################################################

void setup ()
{
    Serial.begin(9600);

    // ---- RTC ------------------------------

    Serial.print("Initialising RTC...");
    #ifdef AVR
        Wire.begin();
    #else
        Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
    #endif
        rtc.begin();

    if (! rtc.isrunning()) {
        Serial.println(" RTC failed!");
        return;
    }

    // Set the time to the compile time of this script. It's off by a few
    // seconds, but good enough for hourly monitoring.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println(" RTC OK.");

    // Let's remember the hour, and just wait for it to tick over.
    now = rtc.now();

    Serial << "RTC time is: " << datestamp(now) << "T" << timestamp(now) << endl;

    Serial.println("RTC OK!");
    Serial.println("------------------------");

    // Give the initialisation some time to print to serial, as well as
    // give us some visual indication that everything is okay.
    ledBlink(10, 100);
}

void loop ()
{
    now = rtc.now();
    Serial << datestamp(now) << "T" << timestamp(now) << endl;
    delay(1000);
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
    // utility function for printing leading zeroes.
    String output;
    
    if (digits < 10)
        output += "0";
        
    output += digits;
    return output;
}

void ledBlink(int flashes, int interval)
{
    for(int i=0; i<flashes; i++)
    {
        digitalWrite(ledPin, HIGH);
        delay(interval);
        digitalWrite(ledPin, LOW);
        delay(interval);
    }
}
