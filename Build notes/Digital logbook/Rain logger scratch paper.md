# Logging rain gauge #project

## Use case ##
This device will be placed in the field and allowed to log rain data for at least 1 year and a maximum of 3 years.

## Requirements ##
1. Uses a common battery type like AA or 9 V, allowing for global field resupply.
2. Has a battery life of at least one year.
3. Records rainfall in mass/hour (1 mm == 1 kg per m^2 per hour). The recommended resolution of 0.2 mm is therefore 200 mL per hour.
4. Chassis and moving parts built from off-the-shelf materials with a minimum of hand-working.

## Mathing it up ##
For a 20 cm funnel:

      π x r^2
    = π x 0.1^2 m
    = 0.0314159 m^2 funnel area.

      1 m^2 / funnel area
    = 31.8309886 scaling factor (# funnels inside 1 m^2)

By multiplying a list of potential tipping weights with my scaling factor above, we can find how many mm of rain each bucket tip records. With a 20 cm funnel, 1 mm of rain roughly corresponds with a tip weight of 32 g. With a 10 cm funnel, it corresponds to 8 g (exact numbers in spreadsheet).

## Prototype materials list ##

### From the dollar store ###
| Qty | Item                           | Cost (AUD) |
|-----|--------------------------------|-----------:|
| 1   | Smart-Link 'Store Jar' large   | 2.50       |
| 1   | SAS Automotive 4 pc funnel set | 2.00       |
| 1   | Homeware Hardware 20 cm funnel | 2.00       |
| 2   | Party Central 4 pk candy scoop | 6.00       |
| 1   | Eagiestar 'Epoxy Steel'        | 2.00       |
|     |                                |            |
|     |                      **Total** | **14.50**  |

### From Bunnings ###
| Qty | Item                        | Cost (AUD) |
|-----|-----------------------------|-----------:|
| 1   | 36" of 3/16" threaded rod   | 03.00      |
| 1   | 3/16" bolt and nut 22 pc set| 04.00      |
| 1   | 3/16" brass washer 25 pc set| 03.32      |
|     |                             |            |
|     |                   **Total** | **10.32**  |

## Equipment list ##

### From the dollar store ###
| Qty | Item                | Cost (AUD) |
|-----|---------------------|-----------:|
| 1   | Butane pencil torch | 06.00      |
| 1   | Mini hacksaw        | 03.00      |
| 1   | Mini hacksaw blades | 02.00      |
|     |                     |            |
|     | **Total**           | **11.00**  |

## Order of actions ##
1. Cut off the bottom of the jar with a standard disposable-blade cutter. Discard cut-off.
2. Cut off the handles from two candy scoops with mini hacksaw. File the cut down until level with the scoop body. Discard cut-off handles.
3. Scoop bodies need to be joined to make the double-sided bucket. I'll try Plastimake. A hole for the pivot axle can also be made inside the Plastimake, combining two jobs into one.
    1. First, make a template to ensure that the scoops can be joined at a repeatable angle (40°). Cut three squares from corrugated cardboard, cut a 50° wedge out of each square, and then glue them all face-to-face with epoxy. Cover the cut faces with duct tape; this is your spacer mould.
    1. Soften 5 grams of Plastimake in boiling water. When soft, place it into newly-boiled water to get it as hot as possible. Quickly fold it into a cube, then force the cube into the mould to produce a wedge of Plastimake.
    1. While still soft, push a large nail through the Plastimake wedge from both sides, wiggling the nail to enlarge the hole. This is the hole for the axle.
4. When the wedge is completely solidified, drill out the hole to fit your axle loosely.
5. Soften the sides of the wedge by dipping briefly in boiling water, then press-fit onto the scoops to make the shape of the wedge conform to the shape of the scoop. If desired, the bond can be made stronger with epoxy.

**Wrong turn:** This design seems easy when you're looking at a bunch of scoops, but it's actually really tough to get the scoops centered and place the axle hole uniformly and so on. It took me basically 4 hours to figure out the scoops. Tomorrow I will buy some aluminum sheet (or some cans of soft drink!) and just scratch-make the bucket assembly.

**Another wrong turn:** The jar I bought is also too small. I will grab a PVC pipe and cap tomorrow too.

## Quick reference for rain logger maths ##

### Heaviest one-day rainfall in the world ###
2,493 mm (12,465 0.2 mm bucket tips, ~ 9 per min)

### Heaviest one-day rainfall in the Blue Mountains ###
285 mm (1,425 0.2 mm bucket tips, ~ 1 per min)

### SD card write time ballpark ###
Let's call it 0.01 ms to write 1 byte. Therefore the data string:

    2015-03-21T15:26:47    540\n(24 bytes)

Can be written in 0.24 ms.

### Milliseconds in 1 hour ###
3,600,000 ms

### List of power draws ###

| Board                  | State       | Current draw (mA) |
|------------------------|-------------|------------------:|
| Pro Mini               | Running     | 16.00             |
|                        | Sleeping    | 0.48              |
| MicroSD module         | Writing     | 1.47              |
|                        | Sleeping    | 1.34              |
| Real-Time Clock module | Reading     | ?                 |
|                        | Sleeping    | 1.70              |
| Fudge factor           | All states  | 1.00              |
|                        |             |                   |
| **Total**              | **Running** | **20.17**         |
|                        | **Sleeping**| **4.52**          |

The heaviest rain ever gave 540 tips per hour. At 0.23 ms per tip, that's 0.0000345 of each hour writing at 20.17 mA. That leaves 0.9999655 of each hour doing nothing at 4.52 mA.

Therefore, each hour drains:

			0.000695865 mA per hour writing
			4.519844060 mA per hour idling
	TOTAL = 4.520539925 mA per hour

Therefore, a 2400 mAh alkaline battery gives 530 hours of perfect operation (22 days). The numbers are basically identical for the Blue Mountains rainfall record.

So the biggest drain here is not the time spent running the program, but the time spent sleeping. Therefore my efforts should concentrate on getting idle drain as low as possible.

## Data logger costing ##

| Qty | Item                   | Supplier     |  Cost (AUD) |
|-----|------------------------|--------------|------------:|
| 12  | Davis rain collector   | DavisNet     |     1335.96 |
| 12  | AA battery holder      | LittleBird   |       12.96 |
| 12  | 5 V step-up module     | eBay         |       47.16 |
| 12  | Arduino Pro Mini 5 V   | LittleBird   |      141.00 |
| 12  | 5 x 7 cm proto board   | eBay         |       35.88 |
| 12  | Real-time clock module | eBay         |       44.04 |
| 12  | MicroSD module         | eBay         |       63.24 |
| 12  | 4 GB microSD card      | eBay         |       59.28 |
| 12  | Reed switch module     | eBay         |       45.72 |
| 12  | 64 x 58 x 35 mm box    | LittleBird   |       71.40 |
| 36  | Duracell AA batteries  | eBay         |       29.99 |
|     |                        |              |             |
|     |                        | **Total**    | **1886.63** |
|     |                        | **Fudged**   | **2000.00** |
|     |                        | **Per unit** |  **166.67** |

## Data logger flow ##

1.	Watchdog timer starts (poll every 6 minutes).
2.	Arduino goes to sleep.
3.	Water bucket tips, and a magnet on the bucket sweeps across a reed switch. Reed switch is connected to voltage, and to a digital input pin set up to interrupt sleep.
4.	When the reed switch closes, the digital pin is set high.
5.	Arduino wakes up due to high interrupt pin.
6.	Arduino increments an internal counter (TipCounter), and then goes back to sleep as quickly as possible.
7.	Meanwhile, watchdog timer is asking every 8 minutes:
	1. Does the RTC say that 1 hour has elapsed?
	2. If no, go back to sleep.
	3. If yes:
	    - Write TipCounter to the SD card with the date and time.
	    - Set TipCounter = 0
	    - How many seconds off from the hour is the watchdog? Set the next watchdog timers shorter to sync them.
	    - Go back to sleep.

## Things to do for data logger ##

### For 6 April 2015 ###

- ☑ Learn how to write to microSD.
	- Write:	SD	→	ReadWrite
- ☑ Learn how to set and get RTC.
	- Set:	Time	→	TimeSerial
	- Get:	DS1307RTC	→	ReadTest
- ☒ Learn how to use reed switch to set a pin high.
	- That actually doesn't work --- you need to set the pin low.
- ☑ Learn how to put Arduino to sleep and wake it up with the reed switch.
- ☑ Learn how to use a watchdog timer.
- ☑ Learn how to run an Arduino from 2x AA batteries.
- ☑ Learn how to run RTC from 2x AA batteries.
- ☑ Learn how to write to microSD from 2x AA batteries.

### For 10 April 2015 ###

- ☐ Learn how to measure milliamps with my cheapo multimeter.
	- Ahaha I blew the fuse, that's why I was having trouble.
	- ☑ Make a fixed-voltage power regulator to help me calculate current.
		- I made one with an LM317, Vout= 5.04V, R1=330R, R2=1000R.
- ☐ Experiment with different sleep modes to see if my code still works.
- ☑ Solder headers onto Arduino Pro Mini and Nanos.
- ☐ Transplant modules onto Pro Mini and see if everything works.
- ☐ Design protoboard layout.

## Data logger pinout ##

|         | Module | Arduino |
|---------|--------|---------|
| RTC     |        |         |
|         | Gnd    | GND     |
|         | VCC    | 5V      |
|         | SDA    | A4      |
|         | SCL    | A5      |
|         |        |         |
| MicroSD |        |         |
|         | Gnd    | GND     |
|         | VCC    | 5V      |
|         | MISO   | D12     |
|         | MOSI   | D11     |
|         | SCK    | D13     |
|         | CS     | D10     |

## File naming scheme ##

File names need to be 8 characters or fewer, which is poo. But anyway:

    123456789
    ---------
    20150410

A condensed date naming scheme works, I think, because of my backup strategy:

## Backup strategy ##

I will have one large log (LOGFILE.CSV) where all of my data is written. Every day, the contents of LOGFILE.CSV get dumped into a new file named with the YYYYMMDD.CSV scheme. If LOGFILE.CSV gets corrupted, then the daily dumps might still be alright!
