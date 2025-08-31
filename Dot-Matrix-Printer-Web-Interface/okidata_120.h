
#ifndef OKIDATA_120_H
#define OKIDATA_120_H

#include <Arduino.h>
#include "src/iec_driver.h"

enum class PrintMode : int {
    Graphic = 0,
    Business = 7
};

class Okidata120 
{
public:
    Okidata120(int attention_pin, int reset_pin, int clock_pin, int data_pin) 
    {
    }

    void begin(PrintMode mode, int printer_addr = 4) {
        iec.begin(printer_addr);
        iec.sendReset();

        // Initialize CBM communication
        iec.sendHeader((int)mode);
    }

    // Like print, but doesn't attempt to auto-remap unsupported characters. Use at your own risk.
    void write(char c) {
      iec.send(c, false);
    }

    void print(char c) {
      iec.send(remapCharset(c), false);
    }

    void print(const char* data, int max_len) {
        if (data[0] == '\0' || max_len <= 0) {
            return;
        }

        for (int i = 0; i < max_len; i++) {
            if (data[0] == '\0') break;
            //bool lastChar = ((i + 1) == max_len) || (data[1] == '\0');
            iec.send(remapCharset(data[0]), false);
            data++;
        }
    }

    void print(String str) { print(str.c_str(), str.length()); }

    void println() {
      print(newline.c_str(), newline.length());
    }

    void println(char c) {
      iec.send(remapCharset(c), false);
      print(newline.c_str(), newline.length());
    }

    void println(const char* data, int max_len) {
        if (data[0] != '\0' && max_len > 0) {
            for (int i = 0; i < max_len; i++) {
                if (data[0] == '\0') break;
                iec.send(remapCharset(data[0]), false);
                data++;
            }
        }

        print(newline.c_str(), newline.length());
    }

    void println(String str) { println(str.c_str(), str.length()); }

    void setNewline(String newline_char) {
        this->newline = newline_char;
    }

private:
    IEC iec;
    String newline = "\r";

    char remapCharset(char input) {
        if ((input >= ' ' && input <= 'Z') || input == '\n' || input == '\r') {
            return input;
        } else if (input >= 'a' || input <= 'z') {
            return (input - 'a') + 'A';
        } else {
            return ' ';
        }
    }
};

#endif