/*
 * Brother HR-5C thermal matrix printer driver for Arduino Diecimila
 *
 *                    Version 1.0, 11/05/2009
 *                    tapani (at) rantakokko.net
 *               (c) Copyright 2009 Tapani Rantakokko
 *
 *
 * Commodore 64 microcomputer used to be very popular in its own time.
 * Many kinds of peripheral devices were made for it, including printers.
 * Printers were connected to C-64's non-standard serial port, just like
 * more common floppy disk drives (remember the famous 1541?).
 *
 * This module provides an API for using Commodore 64's printers from
 * Arduino. It has been developed and tested with Brother HR-5C thermal
 * matrix printer, but should work (perhaps after some adjustments) with
 * other Commodore 64 compatible printers too.
 *
 * Note that you cannot simply connect Arduino's digital I/O pins to the
 * printer's serial port, you have to use a buffer chip similar to what
 * was used in the orignal devices. You can get one from a broken C-64
 * hardware (that's what I did), or buy a compatible one. The original
 * was SN7406N, but SN7407 should work fine as well (not tested, though).
 * Note that this chip also inverts all (output) signals, which is handled
 * in the code.
 *
 * Happy hacking!
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

 // NOTE: Original file has been modified into a custom library for simpler use

 #ifndef LIB_CBM_H
 #define LIB_CBM_H

 #include <Arduino.h>

enum class PrintMode : int {
    Graphic = 0,
    Business = 7
};

class CBM {
public:
    CBM(int attention, int reset, int clock, int data_in, int data_out) {
        this->pin_attn = attention;
        this->pin_reset = reset;
        this->pin_clk = clock;
        this->pin_data_in = data_in;
        this->pin_data_out = data_out;
    }

    // Printer addr is usually 4 or 5
    void begin(PrintMode mode, int printer_addr = 4) {
        pinMode(this->pin_reset, OUTPUT);
        pinMode(this->pin_attn, OUTPUT);
        pinMode(this->pin_clk, OUTPUT);
        pinMode(this->pin_data_out, OUTPUT);
        pinMode(this->pin_data_in, INPUT);

        // Initialize printer
        // Set I/O lines to idle/inactive
        // CBM signal lines are active low, but we have an inverter chip!
        digitalWrite(this->pin_reset, LOW);
        digitalWrite(this->pin_attn,   LOW);
        digitalWrite(this->pin_clk,   LOW);
        digitalWrite(this->pin_data_out,  LOW);

        // Reset the CBM peripheral device
        digitalWrite(this->pin_reset, HIGH);
        delay(100);
        digitalWrite(this->pin_reset, LOW);
        delay(3000);

        // Initialize CBM communication
        cbm_serial_begin(mode, printer_addr);
    }

    void setNewline(String newline_char) {
        this->newline = newline_char;
    }

    // Returns 1 if device is ready, and 0 if device is busy.
    bool deviceReady()
    {
        // Note: DATA in is *not* inverted by HW.
        return digitalRead(this->pin_data_in);
    }

    void print(const char* text, int length = -1) {
        if (length < 0) {
            while (*text != '\0') {
                cbm_serial_write_frame(cbm_switch_case(*text), 0);
                text++;
            }
        } else {
            for (int i = 0; i < length; i++) {
                if (text[i] == '\0') break;
                cbm_serial_write_frame(cbm_switch_case(text[i]), 0);
            } 
        }
    }

    void print(String text) {
        print(text.c_str(), text.length());
    }

    void println(const char* text, int length = -1) {
        print(text, length);
        print(this->newline);
    }

    // Prints a self-test to verify the printer is working
    void printSelfTest() {
        println("Hello World, here are the chars supported by the printer:");
        for (int i=0; i<16; i++)
        {
            cbm_serial_write_frame(i+32, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+48, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+64, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+80, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+96, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+112, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+160, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+176, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+192, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+200, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+224, 0);
            cbm_serial_write_frame(32, 0);
            cbm_serial_write_frame(i+240, 0);
            cbm_serial_write_frame(10, 0);
            cbm_serial_write_frame(13, 0);
        }
    }

private:
    int pin_attn;
    int pin_reset;
    int pin_clk;
    int pin_data_in;
    int pin_data_out;

    // by default, most printers automatically add a newline when a cr is sent
    String newline = "\r";

    // Begins communication sequence on CBM serial line.
    void cbm_serial_begin(PrintMode mode, int printer_addr)
    {
        // Header begins (device acks by setting DATA low).
        digitalWrite(this->pin_attn, HIGH);
        delayMicroseconds(2000); // 2 ms
        digitalWrite(this->pin_clk, HIGH);
        delayMicroseconds(2000); // 2 ms

        // Write listener address.
        cbm_serial_write_frame((unsigned char)(0x20 + printer_addr), 0);

        // Write secondary address.
        cbm_serial_write_frame((unsigned char)(0x60 + (int)mode), 0);

        // Header ends.
        delayMicroseconds(20);
        digitalWrite(this->pin_attn, LOW); // ATN inactive
    }

    /*
    // Ends communication sequence on CBM serial line.
    void cbm_serial_end()
    {
    //TODO is this even needed?

    // End communication
    digitalWrite(CBM_ATN_OUT, HIGH);
    delayMicroseconds(20);

    // Write unlisten command.
    cbm_serial_write_frame((char)(0x3F), 0);

    digitalWrite(CBM_ATN_OUT, LOW);
    delayMicroseconds(100);
    digitalWrite(CBM_CLK_OUT, LOW);
    }
    */

    // Writes one data frame to CBM serial line.
    void cbm_serial_write_frame(unsigned char data, int last_frame)
    {
        // Begin new frame.
        digitalWrite(this->pin_clk, LOW);

        // Device sets DATA high when ready to receive data (not busy).
        while(!deviceReady()) delayMicroseconds(10);

        // Last frame recognition
        if (last_frame == 1)
        {
            // TODO is this even needed?
        /*   
            delayMicroseconds(250); // <250us, this IS the last byte
            // ... wait until printer sets data low...
            while(deviceReady) {
            delayMicroseconds(10);
            }
            // ... wait until printer sets data high...
            while(!deviceReady()) {
            delayMicroseconds(10);
            }
            delayMicroseconds(20);
        */   
        }
        else
        {
            delayMicroseconds(40); // <200us, this is not the last byte
        }

        // Write the actual data byte.
        cbm_serial_write_byte(data);

        // End frame.
        digitalWrite(this->pin_clk, HIGH);
        digitalWrite(this->pin_data_out, LOW); // DATA high (inverted)
        delayMicroseconds(20);

        // Device sets DATA low when it begins processing the data (busy).
        while(deviceReady()) delayMicroseconds(10);

        // Delay between frames.
        delayMicroseconds(100);
    }

    // Writes one bit (data byte's LSB) to CBM serial line.
    void cbm_serial_write_bit(unsigned char data)
    {
        digitalWrite(this->pin_data_out, (~data) & 0x01);
        delayMicroseconds(20);
        digitalWrite(this->pin_clk, HIGH);
        delayMicroseconds(20);
        digitalWrite(this->pin_clk, LOW);
        delayMicroseconds(20);
    }

    // Writes one byte to CBM serial line (LSB first, MSB last).
    void cbm_serial_write_byte(unsigned char data)
    {
        cbm_serial_write_bit( data       & 0x01); // 1st bit (LSB)
        cbm_serial_write_bit((data >> 1) & 0x01); // 2nd bit
        cbm_serial_write_bit((data >> 2) & 0x01); // 3rd bit
        cbm_serial_write_bit((data >> 3) & 0x01); // 4th bit
        cbm_serial_write_bit((data >> 4) & 0x01); // 5th bit
        cbm_serial_write_bit((data >> 5) & 0x01); // 6th bit
        cbm_serial_write_bit((data >> 6) & 0x01); // 7th bit
        cbm_serial_write_bit((data >> 7) & 0x01); // 8th bit (MSB)

        // 1st bit (data & 0x01) is LSB
        // 8th bit (data & 0x80) is MSB
        //for (int i = 0; i < 8; i++) {
        //    cbm_serial_write_bit((data >> i) & 0x01);
        //}
    }

    // Switches case for alpha values (they must be reversed).
    int cbm_switch_case(char data)
    {
      // TODO add support for lower-case data
      // this lookup is designed around the okidata 120. 
      // It may vary for other printers

      // convert special chars to spaces
      if (data < 32) {
        return 32;
      }

      if (data >= 'a' && data <= 'z') {
        // convert to upper case
        return data - 32;
      }

      // data > 'z'
      return 113;

      /*
        if (data >= 0x41 && data <= 0x5A)
        { // Convert from lower to upper (a->A)
            return data + 32;
        }
        else if (data >= 0x61 && data <= 0x7A)
        { // Convert from upper to lower (A->a)
            return data - 32;
        }
        else
        { // Not an alpha value, no conversion needed
            return data;
        }
      */
    }
};

 #endif
