#include <LowPower.h>

volatile bool trigone = false;
volatile bool trigtwo = false;

void setup() {
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    attachInterrupt(0, recordTip, FALLING);
    attachInterrupt(1, alarm, FALLING);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  
  if (trigone == true)
  {
      ledBlink(6, 4, 250, 250, 250, 250);
      trigone = false;
  }
  
  if (trigtwo == true)
  {
      ledBlink(7, 4, 250, 250, 250, 250);
      trigtwo = false;
  }
  
  if (trigone == true) // I check trigone twice in case it's been tripped during trigtwo's handling.
  {
      ledBlink(6, 4, 250, 250, 250, 250);
      trigone = false;
  }
  
  ledBlink(6, 1, 100, 100, 100, 100);
  ledBlink(7, 1, 100, 100, 100, 100);
  ledBlink(6, 1, 100, 100, 100, 100);
  ledBlink(7, 1, 100, 100, 100, 100);
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
