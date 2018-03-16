# Using and maintaining Sneesl

## Diagnostic buzzer sounds

### Error sounds

**3 beeps, each the same pitch**
Low battery voltage, or circuit error. Replace the batteries and/or check the circuit for loose or shorted wires.

**2 beeps, one short and one long, each the same pitch**
SD card access error. The card couldn't be accessed or written to. Remove the batteries and then try removing and reinserting the SD card.

### All-okay sounds

**Four notes, rising pitch**
Power-On Self-Test found no problems, the logger is now ready for input. Tip the buckets manually to confirm that they move freely before leaving the site.

**Single beeps**
Played when a bucket tip is detected and added to the running count. A higher-pitched version is played when the running count is recorded to the logfile.


## Logged data format

By default, Sneesl writes to the logfile hourly in this format:

| Time                | Tips | mV   | Event  |
|---------------------|------|------|--------|
| 2016-12-24T12:29:07 | 0    | 4483 | Reboot |
| 2016-12-24T13:00:00 | 0    | 4630 | Normal |
| 2016-12-24T14:00:00 | 0    | 4278 | Normal |
| 2016-12-24T15:00:00 | 3    | 4295 | Normal |
| 2016-12-24T16:00:00 | 7    | 4295 | Normal |
| 2016-12-24T17:00:00 | 34   | 4295 | Normal |
| 2016-12-24T18:00:00 | 16   | 4278 | Normal |
| 2016-12-24T19:00:00 | 3    | 4395 | Normal |
| 2016-12-24T20:00:00 | 0    | 4611 | Normal |
| 2016-12-24T21:00:00 | 0    | 4593 | Normal |

- `Time` is in ISO-8601 format for easy parsing with software.
- `Tips` is the number of bucket tips recorded in the preceding hour. My buckets hold 0.2 mm of rain, so the rainfall experienced between 4 and 5 PM is `34 × 0.2 = 6.8 mm`.
- `mV` is the estimated voltage available at the time of recording. Note that because this is a (very) rough estimation, the values fluctuate quite a bit. This fluctuation is accounted for in Sneesl's voltage checking function.
- `Event` marks the type of log written. _Reboot_ events are written during POST and should be omitted from your results.

## Maintaining Sneesl

- Keep the data loggers dry and sheltered. Mine fit inside the rain collector's housing, which is very convenient.
- Make sure that the wire leading to the data logger does not interfere with the motion of the buckets.
- I had several instances where dust and debris clogged the rain collector and stopped all readings. It pays to check your collectors regularly during the rainy season to make sure this isn't happening.
- Be careful of spiders that make their homes inside the rain collector.
- RTC units can drift by a few minutes over the course of a year. If this is a problem, you should bring in your Sneesl units once a year to update their internal clocks and/or replace the RTC's onboard batteries.

## Program logic

### Power-On Self-Test

1. The user inserts batteries. Sneesl will run on 3x AA batteries, but it is best to provide 6 AA batteries so that there is a redundant power supply option.
2. Sneesl estimates the available battery voltage.
  - If voltage is <3200 mV, the batteries are discharged below an experimentally-derived safety limit. Below this level, the TinyDuino may randomly 'brownout' and reading/writing to the SD card may destroy the stored data.
  - In case of low voltage, Sneesl will use the buzzer to play an insistent error sound (three identical notes, looping). This error sound will also play if there is error in the circuit that interferes with Power-On Self-Test, like a loose wire.
3. If voltage is above the safety limit, Sneesl tries to create a new logfile (if one doesn't exist already) and then tries to write a new observation to the file (marked as a 'Reboot' event).
  - If logfile creation or writing fail, then Sneesl's buzzer will play an SD warning sound (short-long, short-long — imagine it's saying, "Ess Dee! Ess Dee!").
4. Sneesl sets an alarm telling it when to wake up (by default, `RTC.setAlarmRepeat(EVERY_HOUR);`), and then goes into deep sleep to save power.

### In the event of rain

1. If the rain collector's bucket is tipped, the pulse created by the switch wakes Sneesl.
2. Sneesl checks its voltage again, making sure that it is safe to write to the Arduino's onboard memory.
3. The bucket tip is added to a running count.
4. Sneesl goes back to sleep.

### When the alarm goes off

1. The RTC alarm wakes Sneesl.
2. It checks its voltage again to make sure that it is safe to access the SD card.
3. Sneesl records the number of bucket tip events that it counted since the last alarm (marked as a 'Normal' logging event). A confirmation sound is played.
4. The running count is cleared and Sneesl goes back to sleep.
