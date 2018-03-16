#pragma region "#### Includes #################################################"

// General includes
#include <LowPower.h>

// RTC includes
#include <Wire.h>
#include <DSRTCLib.h>

// SD includes
#include <SPI.h>
#include <SD.h>

// DHT includes
#include <dht.h>

#pragma endregion

#pragma region "#### Variable initialisation ##################################"

// This is the header row placed at the top of every log file.
const String headerRow = "Time,Tips,RelHumPct,TempC,mV,Event";

volatile bool tipDetected = false;
volatile bool alarmDetected = false;

// This value is set every time hasEnoughPower() runs. This is so that the VCC reading
// that the logger actually uses to decide whether to write or not to write 
int vccReading = 0; 

int ALARM_PIN = 3;
int INT_NUMBER = 1;
int BUZZER = 7;

DS1339 RTC = DS1339(ALARM_PIN, INT_NUMBER);

// For SD
const int chipSelect = 10;
File logFile;

// For DHT
#define dht_dpin A0
dht DHT;

// For internal logic
int TipCounter = 0;

#pragma endregion

void buzz(int PIN, int code)
{
	switch (code)
	{
	case 1: // SD access error
		tone(PIN, 4978, 30);
		delay(250);
		noTone(PIN);
		tone(PIN, 4978, 120);
		delay(1000);
		noTone(PIN);
		break;
	case 2: // Rain detected and power remaining
		tone(PIN, 1000, 60);
		delay(62);
		noTone(PIN);
		break;
	case 3: // Log written
		tone(PIN, 2000, 60);
		delay(62);
		noTone(PIN);
		break;
	case 4: // All okay
		tone(PIN, 1000, 60);
		delay(62);
		noTone(PIN);
		tone(PIN, 2000, 60);
		delay(62);
		noTone(PIN);
		tone(PIN, 3000, 60);
		delay(62);
		noTone(PIN);
		tone(PIN, 4000, 60);
		delay(62);
		noTone(PIN);
		break;
        case 5: // Rain detected, sounds before the power check
		tone(PIN, 3000, 30);
		delay(32);
		noTone(PIN);
                break;
	default:
		tone(PIN, 4978, 30);
		delay(250);
		noTone(PIN);
		tone(PIN, 4978, 30);
		delay(500);
		noTone(PIN);
		tone(PIN, 4978, 30);
		delay(1000);
		noTone(PIN);
		break;
	}
}

void setup()
{
	// ---- 'Rain collector' -----------
	pinMode(2, INPUT);
	digitalWrite(2, HIGH);
	attachInterrupt(0, recordTip, FALLING);
	
	// ---- Alarm setup ----------------------
	pinMode(ALARM_PIN, INPUT);
	digitalWrite(ALARM_PIN, HIGH);
	attachInterrupt(1, alarm, FALLING);
	
	// ---- Indicator lights -----------------
	pinMode(BUZZER, OUTPUT);
	pinMode(BUZZER, LOW);

	// ---- SD Card --------------------------
	pinMode(8, INPUT_PULLUP);
	pinMode(9, INPUT_PULLUP);
	pinMode(10, OUTPUT);

	if (!SD.begin(10))
	{
		while (true)
		{
			buzz(BUZZER, 1); // SD access error
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
				logFile.println(headerRow); // Write a header row into a new file.
			}

			logFile.println(buildOutput("Reboot"));
			logFile.close();
		}
		else
		{
			while (true)
			{
				buzz(BUZZER, 1); // SD access error
			}
		}
	}

	// ------------------------------------------------
	
	Wire.begin();
	RTC.start();
	
	RTC.enable_interrupt();
	
	RTC.setMinutes(0);
	RTC.setSeconds(0);
	RTC.setAlarmRepeat(EVERY_MINUTE);
	RTC.writeAlarm();

	buzz(BUZZER, 4); // All okay
}
	
void loop()
{
	LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

	if (tipDetected == true)
	{
		tipDetected = false;
                buzz(BUZZER, 5); // Rain detected, sounds before the power check.
                
                if (hasEnoughPower())
                {
        		TipCounter++;
        		buzz(BUZZER, 2); // Rain detected
                }
	}
	
	if (alarmDetected == true)
	{
		RTC.clear_interrupt();
		alarmDetected = false;
		
		logWrite("Normal");
		TipCounter = 0;
	}
}

void recordTip()
{
	tipDetected = true;
}

void alarm()
{
	alarmDetected = true;
}

String buildOutput(String eventType)
{
	delay(1000); // DHT11 spec mandates a 1-second delay to get past initial sensor instability upon powering up.
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
	output += vccReading;
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

	delay(20); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC)); // measuring

	uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
	uint8_t high = ADCH; // unlocks both

	long result = (high << 8) | low;

	result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result; // Vcc in millivolts
}

bool hasEnoughPower()
{
	// This function checks whether the available power is at least 2.8 V, which is what the TinyDuino's microSD shield needs to safely operate.
	// This should get called as a condition for every SD card access.
        vccReading = int(readVcc());
        
        // Originally set to a 2800 mV threshold, but the RTC seemed to be really sketchy at brownout.
	if (vccReading >= 3000)
		return true;
	else
		return false;
}

void logWrite(String eventType)
{
	if (hasEnoughPower())
	{
		logFile = SD.open("logfile.csv", FILE_WRITE);

		if (logFile)
		{
                        // I have noticed that the Vcc recorded in the log can be up to 260 mV less than 
                        // the Vcc that hasEnoughPower() found. Could be ADC settling time or could be
                        // the fact that the SD module is being accessed. That's why I now set a global
                        // vccReading variable instead of accessing readvcc() again for the log entry.
			logFile.println(buildOutput(eventType));
			logFile.close();
		        buzz(BUZZER, 3); // Log written
		}
		else
		{
			buzz(BUZZER, 1); // SD access error
		}
	}
}
