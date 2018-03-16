# Sneesl, a TinyDuino-based data-logger for rain collectors

## What is _Sneesl_?

**Sneesl** <sup>1</sup> is a battery-powered data-logging device built on [the TinyDuino plaform](https://tinycircuits.com/pages/tinyduino-overview), designed to record pulses from a tipping-bucket rain collector. These rain collectors are basically large funnels that drain into two small scoops, set back-to-back on a central pivot. When the scoop is full, it tips over and empties itself, raising the other scoop so that it can fill up. The 'tipping over' motion passes a magnet over a reed switch, which is detected and recorded by Sneesl.

Notes and BOM for building a Sneesl data-logger can be found in `Building Sneesl.md`.

Information about how Sneesl works and what kind of data it outputs can be found in `Using Sneesl.md`.

## License and citation

I dedicate Sneesl to the public domain. If you use it for scientific work, please cite:

> Quintans, DJ 2015. Sneesl: An Arduino-based data-logger for rain collectors. Microprocessor software. https://github.com/DesiQuintans/sneesl-rain-logger

## Features that Sneesl has

- **6+ months battery life from easy-to-find AA batteries.** It takes 6 batteries in a parallel 3×2 configuration for redundancy. In practice I have sometimes left Sneesl in the field for 9+ months, but I prefer to maintain them more regularly.
- **Data storage on microSD.** MicroSD cards are relatively cheap and are reliable for field use. They can be read in the field using a tablet or laptop, so you can pull down the data without having to bring the logger back to the lab or having to use an expensive reader.
- **Integrated battery life checks.** Sneesl ensures that the battery's voltage is stable enough to safely write data to the SD card. It also records battery life as part of the logging process.
- **Power-On Self-Test.** Sneesl uses a buzzer to warn you if something is wrong (e.g. SD card inserted incorrectly, battery voltage low, unknown problem with the circuit), so that you know the logger is okay before you leave the field site.
- **Input-agnostic pulse counting.** If you have any kind of sensor that uses a switch closure as the response, Sneesl can log it.
- **Timed events.** Sneesl does its logging at regular intervals using a real-time clock, so you can modify it to do things like read a humidity/temperature sensor regularly too.

## Features that Sneesl *does not* have

- Since the number of input channels on an Arduino is limited, Sneesl is not the best choice for multiplexing lots of sensors.
- Sneesl could probably do remote monitoring by using a phone or GSM module to transmit information, but I have not tried to actually implement it here.

## Resources included in this repository

- `Build notes/` contains information about actually building Sneesl.
  - `Bill of materials/` contains a parts list with approximate prices.
  - `Circuits/` has the circuit diagrams. Always use the newest one.
  - `Digital logbook/` is mostly me talking to myself inside text files throughout the development process.
  - `Photos/` has unsorted photos of the development, testing, and assembly process.
- `Firmware/` contains the Arduino code for the loggers.
  - `08_LoggerV8/` is a version of `V7` that has some changes to its comments and formatting.
  - `07_LoggerV7/` is the version I have deployed in the field.
  - `06_LoggerV6/`
  - `05_LoggerV5/`
  - `04_Datalogger_TinyDuino/`
  - `03_BareMetal_TinyDuino/`
  - `02_BareMetal_TD_2/`
  - `01_Datalogger/` is the first version, it ran on $1 Arduino Nano clones with extra modules.
- `_Bench testing/` has enclosure tests, visualisations of data, and battery life predictions.
- `_Scratchpad/` contains code that I used during Sneesl's development process.
  - `INTERRUPTS/` was used for prototyping the wake-up process.
  - `LOW POWER/` was used for prototyping the deep-sleep process.
  - `RTC/` is used to set and get Real-Time Clock modules.
  - `SD/` was for testing different SD card readers before I settled on TinyDuino.
  - `SENSORS/` is test code for photocells (to record brightness) and DHT11 sensors (temperature and humidity) which I omitted from the production run of Sneesl.

---

1: _Sneesl_ is [a Scottish word for raining lightly](http://scottishsceptic.co.uk/2014/11/28/scots-more-words-for-rain-than-eskimos-for-snow/).
