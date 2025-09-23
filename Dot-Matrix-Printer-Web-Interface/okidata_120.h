
#ifndef OKIDATA_120_H
#define OKIDATA_120_H

#include <Arduino.h>
#include "src/iec_driver.h"
#include "src/c64_basic.h"

enum class PrintMode : uint8_t {
    Graphic = 0,
    Business = 7
};

class Okidata120 
{
public:
    Okidata120() 
    {
    }

    void begin(PrintMode mode, int printer_addr = 4) {
        this->mode = mode;
        this->addr = printer_addr;

        iec.begin();
        iec.sendReset();

        Serial.println("Starting...");
        String message = "Hello, world! This is a test message.\r";

        IECResult result;
        LogicalFile printer;
        if (!LogicalFile::open(&printer, &iec, printer_addr, (uint8_t)mode))
        {
            Serial.print("Failed to open printer.");
            while(1){}
        }

        for (int i = 0; i < 10; i++)
        {
            // Print example data
            for (int j = 0; j < message.length(); j++)
            {
                uint8_t c = remapCharset(message[j]);
                result = printer.print(&c, 1, (i == 9) && (j + 1 == message.length()));
                if (result != IECResult::Success)
                {
                    Serial.print("Failed to print data: ");
                    Serial.println((int)result);
                    while(1){}
                }
            }
            
        }

        printer.close();
        Serial.println("Done");

        delay(5000);

        Serial.println("Starting 2...");

        if (!LogicalFile::open(&printer, &iec, printer_addr, (uint8_t)mode))
        {
            Serial.print("Failed to open printer2.");
            while(1){}
        }
        uint8_t test_msg[] = { 84, 72, 73, 83, 32, 87, 79, 82, 75, 69, 68, 33, 13 };
        result = printer.print(test_msg, 13, true);
        if (result != IECResult::Success)
        {
            Serial.print("Failed to print data: ");
            Serial.println((int)result);
            while(1){}
        }

        printer.close();
        Serial.println("Done2");

        /*for (int i = 0; i < 10; i++)
        {
            Serial.print("Print ");
            Serial.println(i + 1);
            // Initialize CBM communication
            iec.cmd_Start();
            iec.cmd_Listen(printer_addr);
            iec.cmd_Second((uint8_t)mode);
            iec.cmd_End();

            // Print example data
            for (int j = 0; j < message.length(); j++)
            {
                iec.send(remapCharset(message[j]), j == message.length() - 1, false);
            }
            iec.endTransmission();
            Serial.println("Done");

            delay(500);
        }*/

        /*String str = "Hello I need to print this as a test.\r";
        const char* data = str.c_str();
        int max_len = str.length();

        Serial.println("Print 2");
        initializeCBM();
        for (int i = 0; i < max_len; i++) {
            if (data[0] == '\0') break;
            bool lastChar = ((i + 1) == max_len) || (data[1] == '\0');
            iec.send(remapCharset(data[0]), lastChar);
            data++;
        }
        iec.endTransmission();
        Serial.println("Done");

        delay(1000);

        data = str.c_str();
        Serial.println("Print 3");
        initializeCBM();
        for (int i = 0; i < max_len; i++) {
            if (data[0] == '\0') break;
            bool lastChar = ((i + 1) == max_len) || (data[1] == '\0');
            iec.send(remapCharset(data[0]), lastChar);
            data++;
        }
        iec.endTransmission();
        Serial.println("Done");*/
        while(true){}
    }

    void print(char c) {
        initializeCBM();
        iec.send(remapCharset(c), true);
        iec.endTransmission();
    }

    void print(const char* data, int max_len) {
        if (max_len <= 0 || data[0] == '\0') {
            return;
        }

        initializeCBM();
        for (int i = 0; i < max_len; i++) {
            if (data[0] == '\0') break;
            bool lastChar = ((i + 1) == max_len) || (data[1] == '\0');
            iec.send(remapCharset(data[0]), lastChar);
            data++;
        }
        iec.endTransmission();
    }

    void print(String str) { print(str.c_str(), str.length()); }

    void println() {
      print(newline.c_str(), newline.length());
    }

    void println(char c) {
      print(c);
      print(newline.c_str(), newline.length());
    }

    void println(const char* data, int max_len) {
        print(data, max_len);
        print(newline.c_str(), newline.length());
    }

    void println(String str) { println(str.c_str(), str.length()); }

    void setNewline(String newline_char) {
        this->newline = newline_char;
    }

private:
    IEC iec;
    String newline = "\r";
    PrintMode mode;
    int addr;

    char remapCharset(char input) {
        if ((input >= ' ' && input <= 'Z') || input == '\n' || input == '\r') {
            return input;
        } else if (input >= 'a' || input <= 'z') {
            return (input - 'a') + 'A';
        } else {
            return ' ';
        }
    }

    void initializeCBM()
    {
        iec.cmd_Start(); // Make everything listen to us (we are the controller)
        iec.cmd_Listen(addr); // Tell the printer we want it to listen
        iec.cmd_Second((uint8_t)mode); // Tell the printer which mode to use
        iec.cmd_End(); // Finish the command
    }
};

#endif