# Building a Sneesl data logger

These notes assume that you have basic experience with assembling and soldering an electronic circuit, as well as connecting and programming an Arduino.

## Materials for 1x Sneesl

| Pieces | Object                                  | Source            | Rounded cost (AUD)   |
| ------ | --------------------------------------- | ----------------- | -------------------- |
|        | **Exterior:**                           |                   |                      |
| 1      | Davis tipping-bucket rain collector     | Davis Instruments | 150.00               |
| 1      | Waterproof enclosure, 100×68×50 mm      | AliExpress        | 3.00                 |
| 1      | Spring-loaded toggle latch, 27 mm       | AliExpress        | 1.00                 |
| 4      | Pop rivets for latch                    | Hardware store    | 1.00                 |
|        |                                         |                   | 155.00 (Total)       |
|        | **TinyDuino modules:**                  |                   |                      |
| 1      | Processor board w/o battery holder      | TinyCircuits      | 14.00                |
| 1      | TinyShield MicroSD                      | TinyCircuits      | 14.00                |
| 1      | TinyShield RTC                          | TinyCircuits      | 18.00                |
| 1      | TinyShield Proto Terminal               | TinyCircuits      | 9.00                 |
| 1      | Mounting hardware kit                   | TinyCircuits      | 4.00                 |
| 1      | MicroSD card (8GB) *                    | TinyCircuits      | 10.00                |
| 1      | TinyShield USB (side connector) *       | TinyCircuits      | 15.00                |
|        |                                         |                   | 84.00 (Total)        |
|        | **Assorted electronics:**               |                   |                      |
| 2      | Diode, 1N5819                           | AliExpress        | 1.00                 |
| 1      | Resistor, 4.7k ohms                     | AliExpress        | 1.00                 |
| 1      | Through-hole piezo buzzer               | AliExpress        | 1.00                 |
| 1      | 3.5 mm jack (male/female panel-mount) * | AliExpress        | 1.00                 |
| 1      | Prototyping board (cut to size)         | AliExpress        | 1.00                 |
| 2      | 3x AA battery holders                   | AliExpress        | 1.00                 |
| 6      | AA batteries (decent ones) *            | Reject Shop       | 2.00                 |
|        |                                         |                   | 8.00 (Total)         |
|        | **Other consumables:**                  |                   |                      |
|        | Solid-core and braided wire             |                   |                      |
|        | Solder                                  |                   |                      |
|        | Soldering braid                         |                   |                      |
|        | Printable adhesive labels               |                   |                      |
|        |                                         |                   | 247.00 (Grand total) |

**Re. microSD cards:** I bought the cards from TinyCircuits so that I didn't have to worry about compatibility or counterfeiting. If you want to source your own SD cards, try to find cards that are 1-16 GB in size, since these tend to be more reliable with Arduinos.

**Re. the TinyShield USB programmer:** You only need to buy one or two of these; the USB programmer is not deployed in the field with the rest of the stack.

**Re. the 3.5 mm jack:** You can use any **normally open** connector you want, I just had a lot of 3.5 mm jacks on hand. You *could* wire the rain collector into the logger directly, but it's very convenient to be able to separate them.

**Re. AA battery holders:** Some holders come with an exposed length of wire at the back, linking the cells together. It's smart to cover this wire with tape so that it doesn't short with anything.

**Re. AA batteries:** I can buy a 50-pack of Eveready AA batteries for $12 from my local Reject Shop branch (it's a seconds store). These batteries haven't leaked in any of my 12 loggers, even when the temperature fluctuates from 0–45 °C. I guess you could use rechargeables, but then you add the mental load of having to make sure the new batteries are properly charged. The downside of using disposable batteries is that every 6 months when I replace the batteries, I am left with 72 batteries that are only one-quarter discharged.

## Circuit diagram

Please see `_Build notes/Circuits/` to find circuit diagrams. The .fzz format is openable with [Fritzing](http://fritzing.org). If you don't have Fritzing, you can look at the .png format version.

## Build photos and tips

Please see `_Build notes/Photos/` for mixed photos of the build process that could be helpful. If you browse the documents in `_Build notes/Digital logbook/` could also be instructive.

## Programming Sneesl

You can use the Arduino IDE to program Sneesl.

1. Use `Scratchpad/RTC/Set_RTC_Time` to set the time on the TinyDuino's RTC (real-time clock) shield. Compile the script and then upload it immediately (because it sets the RTC based on the compile time of the script).
2. Compile and upload the latest Sneesl firmware from `_Firmware/`.
