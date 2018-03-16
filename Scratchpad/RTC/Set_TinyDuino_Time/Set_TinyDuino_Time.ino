#include <DSRTCLib.h>
#include <Wire.h>

/*
  Set_TinyDuino_Time, adapted from the DS1339 RTC Example sketch.
 
 */


int ledPin =  13;    // LED connected to digital pin 13
int INT_PIN = 2; // INTerrupt pin from the RTC. On Arduino Uno, this should be mapped to digital pin 2 or pin 3, which support external interrupts
int int_number = 0; // On Arduino Uno, INT0 corresponds to pin 2, and INT1 to pin 3

DS1339 RTC = DS1339(INT_PIN, int_number);

void setup()   {
  pinMode(INT_PIN, INPUT);
  digitalWrite(INT_PIN, HIGH);
  
  pinMode(ledPin, OUTPUT);    
  digitalWrite(ledPin, LOW);

  Serial.begin(9600);
  Serial.println ("Set TinyDuino Time");

  RTC.start(); // ensure RTC oscillator is running, if not already
  
  set_time();
  
  // If the oscillator is borked (or not really talking to the RTC), try to warn about it
  if(!RTC.time_is_set())
  {
    Serial.println("Clock did not set! Check that its oscillator is working.");
  }
}

int read_int(char sep)
{
  static byte c;
  static int i;

  i = 0;
  while (1)
  {
    while (!Serial.available())
    {;}
 
    c = Serial.read();
    // Serial.write(c);
  
    if (c == sep)
    {
      // Serial.print("Return value is");
      // Serial.println(i);
      return i;
    }
    if (isdigit(c))
    {
      i = i * 10 + c - '0';
    }
    else
    {
      Serial.print("\r\nERROR: \"");
      Serial.write(c);
      Serial.print("\" is not a digit\r\n");
      return -1;
    }
  }
}

int read_int(int numbytes)
{
  static byte c;
  static int i;
  int num = 0;

  i = 0;
  while (1)
  {
    while (!Serial.available())
    {;}
 
    c = Serial.read();
    num++;
    // Serial.write(c);
  
    if (isdigit(c))
    {
      i = i * 10 + c - '0';
    }
    else
    {
      Serial.print("\r\nERROR: \"");
      Serial.write(c);
      Serial.print("\" is not a digit\r\n");
      return -1;
    }
    if (num == numbytes)
    {
      // Serial.print("Return value is");
      // Serial.println(i);
      return i;
    }
  }
}

int read_date(int *year, int *month, int *day, int *hour, int* minute, int* second)
{

  *year = read_int(4);
  *month = read_int(2);
  *day = read_int(' ');
  *hour = read_int(':');
  *minute = read_int(':');
  *second = read_int(2);

  return 0;
}

void loop()                     
{
    read_time();
    delay(1000);
}

void set_time()
{
    Serial.println("Enter date and time (YYYYMMDD HH:MM:SS)");
    int year, month, day, hour, minute, second;
    int result = read_date(&year, &month, &day, &hour, &minute, &second);
    if (result != 0) {
      Serial.println("Date not in correct format!");
      return;
    } 
    
    // set initially to epoch
    RTC.setSeconds(second);
    RTC.setMinutes(minute);
    RTC.setHours(hour);
    RTC.setDays(day);
    RTC.setMonths(month);
    RTC.setYears(year);
    RTC.writeTime();
    read_time();
}

void read_time() 
{
  Serial.print ("The current time is ");
  RTC.readTime(); // update RTC library's buffers from chip
  printTime(0);
  Serial.println();

}

void test_basic()
{
  // Test basic functions (time read and write)
  Serial.print ("The current time is ");
  RTC.readTime(); // update RTC library's buffers from chip
  printTime(0);
  Serial.println("\nSetting times using direct method: 1/31/07 12:34:56");
  
    RTC.setSeconds(56);
    RTC.setMinutes(34);
    RTC.setHours(12);
    RTC.setDays(31);
    RTC.setMonths(1);
    RTC.setYears(2007); // 2-digit or 4-digit years are supported
    RTC.writeTime();
    delay(500);  // This is not needed; just making it more clear that we are reading a new result
    RTC.readTime();
    Serial.print("Read back: ");
    printTime(0);
    Serial.println("  (we'll never forget)");
    
    Serial.println("Setting time using epoch seconds: 2971468800 (midnight on 2/29/2064)");
    RTC.writeTime(2971468800u);
    delay(500);  
    RTC.readTime();    
    Serial.print("Read back: ");
    printTime(0);
    Serial.println("  (Happy 21st birthday Carlotta) ");    

    Serial.println("Writing alarm: 8:00am on the 15th of the month.");
    RTC.setSeconds(0);
    RTC.setMinutes(0);
    RTC.setHours(8);
    RTC.setDays(15);
    RTC.setAlarmRepeat(EVERY_MONTH); // There is no DS1339 setting for 'alarm once' - user must shut off the alarm after it goes off.
    RTC.writeAlarm();
    delay(500);
    RTC.readAlarm();
    Serial.print("Read back: ");
    printTime(1);    

    Serial.println("\nWriting alarm: 2:31:05 pm on the 3rd day of the week.");
    RTC.setSeconds(5);
    RTC.setMinutes(31);
    RTC.setHours(14);
    RTC.setDayOfWeek(3);
    RTC.setAlarmRepeat(EVERY_WEEK); // to alarm on matching day-of-week instead of date
    RTC.writeAlarm();
    delay(500);
    RTC.readAlarm();
    Serial.print("Read back: ");
    printTime(1);
    Serial.println("\n");
 }

void printTime(byte type)
{
  // Print a formatted string of the current date and time.
  // If 'type' is non-zero, print as an alarm value (seconds thru DOW/month only)
  // This function assumes the desired time values are already present in the RTC library buffer (e.g. readTime() has been called recently)

  if(!type)
  {
    Serial.print(int(RTC.getMonths()));
    Serial.print("/");  
    Serial.print(int(RTC.getDays()));
    Serial.print("/");  
    Serial.print(RTC.getYears());
  }
  else
  {
    //if(RTC.getDays() == 0) // Day-Of-Week repeating alarm will have DayOfWeek *instead* of date, so print that.
    {
      Serial.print(int(RTC.getDayOfWeek()));
      Serial.print("th day of week, ");
    }
    //else
    {
      Serial.print(int(RTC.getDays()));
      Serial.print("th day of month, ");      
    }
  }
  
  Serial.print("  ");
  Serial.print(int(RTC.getHours()));
  Serial.print(":");
  Serial.print(int(RTC.getMinutes()));
  Serial.print(":");
  Serial.print(int(RTC.getSeconds()));  
}


