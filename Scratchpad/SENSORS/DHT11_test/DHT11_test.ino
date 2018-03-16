#include <dht.h>

#define dht_dpin A0

dht DHT;

void setup() {
  Serial.begin(9600);
  delay(300);
  Serial.println("RelHum(%)\tTemp(C)\n");
  delay(700);
}

void loop() {
  DHT.read11(dht_dpin);
    Serial.print(DHT.humidity);
    Serial.print("\t");
 
    Serial.print(DHT.temperature);
    Serial.println("\t");
  delay(2000);
}
