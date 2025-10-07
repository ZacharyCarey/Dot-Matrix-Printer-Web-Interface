# Commodore 64 Printer Web Interface
Adapter to interface with a CBM dot matrix printer over the IEC interface.

This project is largly based around the Okidata 120 printer, as that is what I own and am trying to get working. If more printers get tested they will be listed here. Eventually I would like for this project to work both directions, as in it can be configured to interface with a dot matrix printer (acting as a C64 host), or connect to a C64 to upload files (acting as a floppy drive).

## Arduino Installation
Copy the libraries from /libs into the Arduino sketchbook library folder.

## Hardware Details
This project is made around a custom PCB designed in EasyEDA and assembled by JLCPCB (their assembly service soldered most of the components, while the connector and Arduino are soldered by hand). For more details on the PCB see the "hardware" folder and the [Ordering from PCB section](#order-from-jlcpcb). 

![Serial port pinout](/docs/SerialBus_pins.gif)

The following pinouts are defined by looking into the end of the connector, looking into the back of the C64 or looking into the port of this adapter:

| Signal | Pin | Remark                                                   |
| ------ | --- | -------------------------------------------------------- |
| SRQ    | 1   | Serial Service Request In                                |
| GND    | 2   | Ground                                                   |
| ATN    | 3   | Attention, for selection of a device during transmission |
| CLK    | 4   | Clock                                                    |
| DATA   | 5   | Data                                                     |
| RESET  | 6   | Reset                                                    |

## Order from JLCPCB
In the "hardware" folder, the zip file that starts with "JLCPCB_" can be uploaded directly to JLCPCB ordering and should contain all relevant info to order the PCB with assembling of the small components. Only the bus pullup resistors aren't assembled, but those should not be needed unless two of these PCB's are directly hooked up to each other.

JLCPCB order details that I successfully used:
- Layers: 2
- Product Type: Consumer electronics
- Deburring/Edge rounding: Yes
- Different Design: 1
- Delivery Format: Single PCB
- PCB Thickness: 1.6mm
- Material Type: FR4 TG135
- Surface Finish: LeadFree HASL
- Outer Copper Weight: 1oz
- Via Covering: Tented
- Min via hole size/diameter: 0.3mm
- Board Outline Tolerance: 0.2mm
- Confirm Production file: No
- Gold Fingers: No
- Castellated Holes: No
- Edge Plating: No
- Blind Slots: No
- PCBA Type: Economic
- Assembly Side: Top Side
- Tooling holes: Added by JLCPCB
- Confirm Parts Placement: No
- Stencil Storage: No
- Fixture Storage: No
- Parts Selection: By Customer
- Photo Confirmation: No
- Conformal Coating: No
- Packaging: Antistatic bubble film
- Solder Paste: High Temp
- Flying Probe Test: No
- Function test: No
- Board Cleaning: No
- Bake Components: No
- Depanel boards: No
- Add paste for unpopulated pad: No
- Nitrogen reflow soldering: Yes
- PCBA remark: no

Also included is the "JLCPCB_BOM" and "JLCPCB_CPL" files that are required for board assembly. JLCPCB will complain about C1 and U1 being unmatched, simply simply select "do not place" and order those parts from Digikey (P/N listed in the regular CSV BOM) so they can be soldered by hand later.

## Additional Resources
- [Okidata 120 handbook](https://archive.org/details/Oki_Okidata_120_Commodore_Handbook/page/n31/mode/2up)
- [ESP32 interactive webpage tutorial](https://randomnerdtutorials.com/esp32-web-server-arduino-ide/)
- [ESP32 auto-updating webpage tutorial](https://esp32io.com/tutorials/esp32-web-server#content_controlling_the_esp32_via_web)
- [ESP32 websockets tutorial](https://esp32io.com/tutorials/esp32-controls-servo-motor-via-web)
- [Original IEC library](https://github.com/Larswad/uno2iec)
- [C64 Wiki port info](https://www.c64-wiki.com/wiki/Serial_Port)
- [C64 IEC API layer](https://www.pagetable.com/?p=1031) and [protocol layer](https://www.pagetable.com/?p=1135)
- [White paper on IEC timings](https://retro-bobbel.de/zimmers/cbm/programming/serial-bus.pdf)