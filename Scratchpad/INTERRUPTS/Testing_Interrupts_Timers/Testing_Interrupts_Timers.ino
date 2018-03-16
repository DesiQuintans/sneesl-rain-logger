
#include <LowPower.h>

const int reedPin = 2;
const int ledPin = 9;
int TipCounter = 0;
volatile bool tipEventDetected = false;
unsigned long timeOfLastTip = 0;

void setup() {
  // put your setup code here, to run once:
    pinMode(ledPin, OUTPUT);
    pinMode(reedPin, INPUT);
    Serial.begin(9600);
    ledBlink(5, 100);
}

void loop() {
    Serial.print(millis());
    Serial.println("\tAttach interrupt.");
    delay(100);
    attachInterrupt(0, tipEvent, LOW);

    Serial.print(millis());
    Serial.println("\tGo to sleep.");
    delay(100);
    LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

    Serial.print(millis());
    Serial.println("\tDetach interrupt.");
    delay(100);
    detachInterrupt(0);

    if (tipEventDetected == true)
    {
        TipCounter++;
        Serial.print(millis());
        Serial.print("\tTipCounter now at ");
        Serial.println(TipCounter);
        delay(100);
        tipEventDetected = false;
    }
}

void tipEvent()
{
    if ((int(millis() - timeOfLastTip) >= 500) || (timeOfLastTip == 0))
    {
        tipEventDetected = true;
        timeOfLastTip = millis();
    }
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
