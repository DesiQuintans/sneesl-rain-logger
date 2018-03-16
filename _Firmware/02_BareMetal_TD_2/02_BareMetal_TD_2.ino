#pragma region "#### Includes #################################################"

// General includes
#include <LowPower.h>
#include <JeeLib.h>

// RTC includes
#include <Wire.h>
#include <DSRTCLib.h>

// SD includes
#include <SPI.h>
#include <SD.h>

// Sensor includes
#include <dht.h>

#pragma endregion

#pragma region "#### Variable initialisation ##################################"

// This is the header row placed at the top of every log file.
const String headerRow = "Time,Tips,Hum%,TempC,mV,Event";

// For error lights
#define LEDPIN 7 // I can't use the built-in LED because it is involved in SPI for the SD shield.

// For RTC
DS1339 RTC = DS1339(3, 1); // Interrupt pin D3, Interrupt number 1 (pin 3).
volatile bool alarmTriggered = false;
#define ALARMPIN 3

// For rain collector
#define RAINPIN 2

// For SD
const int chipSelect = 10;
File logFile;

// For DHT11
#define dht_dpin A0
dht DHT;

// For internal logic
int TipCounter = 0;
volatile bool tipEventDetected = false;
unsigned long timeOfLastTip = 0;

#pragma endregion

// #### Setup ####################################################

/*
	Error codes (Flashing LED on pin 9):

	Fast x3			RTC couldn't be initialised.
	Fast-slow		SD card couldn't be initialised.
	Slow-slow		Couldn't access DHT11.
	2 fast 5 Slow	All OK, starting to log data.
*/

void setup ()
{
	// ---- Error LED ------------------------
  
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);
  
	// ---- Rain collector -------------------
  
	pinMode(RAINPIN, OUTPUT);
	digitalWrite(RAINPIN, HIGH); // When the switch closes, this pin gets pulled low.
  
	// ---- SD Card --------------------------
  
	pinMode(8, INPUT_PULLUP);
	pinMode(9, INPUT_PULLUP);
	pinMode(10, OUTPUT);
  
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
  
	// ---- Alarm ----------------------------
  
	pinMode(ALARMPIN, INPUT);
	digitalWrite(ALARMPIN, HIGH);
  
	// ---- RTC ------------------------------
  
	Wire.begin();
	RTC.start();
  
	// Set the time by running the Set_TinyDuino_Time sketch.
  
	if (!RTC.time_is_set()) {
		while (true)
		{
			// Stop running and just blink the LED fast-fast-fast.
			ledBlink(LEDPIN, 3, 100, 100, 100, 100);
			Sleepy::loseSomeTime(200);
		}
	}

	// ---- Timekeeping ----------------------
  
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
  
	// ---- Prepare external interrupts ------
	attachInterrupt(0, recordTip, LOW);
	attachInterrupt(1, alarm, FALLING);
	RTC.enable_interrupt();
	RTC.setAlarmRepeat(EVERY_MINUTE);
	RTC.setSeconds(0);
	RTC.writeAlarm();
}

// #### Main loop ################################################

void loop ()
{
	LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  
	if (tipEventDetected == true)
	{
		ledBlink(LEDPIN, 4, 250, 250, 250, 250);
		// Oh, I got tipped while I was sleeping! Better write that down.
		TipCounter++;
		tipEventDetected = false;
	}
  
	if (alarmTriggered == true)
	{
		RTC.clear_interrupt();
		ledBlink(LEDPIN, 8, 250, 250, 250, 250);
		// It's a new minute! I can write data! \o/
		logWrite("Normal");
		TipCounter = 0;
		alarmTriggered = false;
	}
}

// #### Function declarations ####################################

// ---- Utility functions ----------------
// I've gotta organise myself with a TOC because Arduino IDE doesn't have code folding! It's 2015, man!
//	1. void	recordTip()
//	2. String  buildOutput(String eventType)
//	3. String  iso8601DateTime()
//	4. String  withLeadingZero(int digits)
//	5. bool	hasEnoughPower()
//	6. void	ledBlink(int ledPinLocation, int flashes, int evenOn, int evenOff, int oddOn, int oddOff)
//	7. long	readVcc()
//	8. float   voltMeasure()

void recordTip()
{
	int timeDifference = int(abs(millis() - timeOfLastTip));
  
	if (timeDifference >= 2000)
	{
		tipEventDetected = true;
		timeOfLastTip = millis();
	}
}

void alarm()
{
	alarmTriggered = true;
}


String buildOutput(String eventType)
{
	delay(1000);
	DHT.read11(dht_dpin);
  
	String output;
	output += iso8601DateTime();
	output += ",";
	output += TipCounter;
	output += ",";
	output += DHT.humidity;
	output += ",";
	output += DHT.temperature;
	output += ",";
	output += readVcc();
	output += ",";
	output += eventType;
	return output;
}

String iso8601DateTime()
{
	RTC.readTime();
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
	// It intentionally blocks to buy time for processes.
	bool even = false;
	for (int i = 0; i < flashes; i++)
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
	while (bit_is_set(ADCSRA, ADSC)); // measuring
  
	uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
	uint8_t high = ADCH; // unlocks both
  
	long result = (high << 8) | low;
  
	result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result; // Vcc in millivolts
}

// ---- SD access functions --------------
// To be done whenever the card is accessed:
//	- Check that there is enough voltage to safely write to the card.
//	- Open whatever streams you need to.
//	- Build the output string.
//	- Write to the stream/s.
//	- Close everything down.

void logWrite(String eventType)
{
	if (hasEnoughPower())
	{
		logFile = SD.open("logfile.csv", FILE_WRITE);
	
		if (logFile)
		{
			logFile.println(buildOutput(eventType));
			logFile.close();
		}
		else
		{
			// Couldn't open.
		}
	}
}
