# Commodore 64 Printer Web Interface
Adapter to interface with a CBM dot matrix printer over the IEC interface.

This project is largly based around the Okidata 120 printer, as that is what I own and am trying to get working. If more printers get tested they will be listed here. Eventually I would like for this project to work both directions, as in it can be configured to interface with a dot matrix printer (acting as a C64 host), or connect to a C64 to upload files (acting as a floppy drive).

## Arduino Installation
Copy the libraries from /libs into the Arduino sketchbook library folder.

## Hardware Details
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

## Additional Resources
- [Okidata 120 handbook](https://archive.org/details/Oki_Okidata_120_Commodore_Handbook/page/n31/mode/2up)
- [ESP32 interactive webpage tutorial](https://randomnerdtutorials.com/esp32-web-server-arduino-ide/)
- [ESP32 auto-updating webpage tutorial](https://esp32io.com/tutorials/esp32-web-server#content_controlling_the_esp32_via_web)
- [ESP32 websockets tutorial](https://esp32io.com/tutorials/esp32-controls-servo-motor-via-web)
- [Original IEC library](https://github.com/Larswad/uno2iec)
- [C64 Wiki port info](https://www.c64-wiki.com/wiki/Serial_Port)