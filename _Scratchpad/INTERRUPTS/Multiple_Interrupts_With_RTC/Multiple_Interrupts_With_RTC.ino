#include <LowPower.h>
#include "DSRTCLib.h"
#include <Wire.h>

volatile bool trigone = false;
volatile bool trigtwo = false;

int ALARM_PIN = 3;
int INT_NUMBER = 1;

DS1339 RTC = DS1339(ALARM_PIN, INT_NUMBER);

void setup()
{
	pinMode(2, INPUT);
	digitalWrite(2, HIGH);
	attachInterrupt(0, recordTip, FALLING);
	
	pinMode(ALARM_PIN, INPUT);
	digitalWrite(ALARM_PIN, HIGH);
	attachInterrupt(1, alarm, FALLING);
	
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	digitalWrite(6, LOW);
	digitalWrite(7, LOW);
	
	// ------------------------------------------------
	
	Wire.begin();
	RTC.start();
	
	RTC.enable_interrupt();
	
	RTC.setMinutes(0);
	RTC.setSeconds(0);
	RTC.setAlarmRepeat(EVERY_MINUTE);
	RTC.writeAlarm();
}
	
void loop()
{
	LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

	if (trigone == true)
	{
		ledBlink(6, 1, 50);
		trigone = false;
	}
	
	if (trigtwo == true)
	{
		RTC.clear_interrupt();
		trigtwo = false;
		
		ledBlink(7, 1, 100);
	}
}

void recordTip()
{
	trigone = true;
}

void alarm()
{
	trigtwo = true;
}

void ledBlink(int ledPinLocation, int flashes, int evenOn, int evenOff, int oddOn, int oddOff)
{
	// Utility function for blinking an LED.
	// It intentionally blocks to buy time for processes.
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

void ledBlink(int ledPinLocation, int flashes, int value)
{
	ledBlink(ledPinLocation, flashes, value, value, value, value);
}
