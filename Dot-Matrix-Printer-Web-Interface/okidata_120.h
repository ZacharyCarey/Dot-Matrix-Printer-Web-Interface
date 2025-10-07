
#ifndef OKIDATA_120_H
#define OKIDATA_120_H

#include <Arduino.h>

enum class PrintMode : char {
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

        Serial2.begin(115200, SERIAL_8N1, 19, 20); // rx=19(A2), tx=20(A3)
    }

    bool print(const char* data, int count) {
        char remap[count];
        remapCharset(data, remap, count);

        bool result = iec_send(remap, count);

        delay(2);
        return result;
    }

    void print(String str) { print(str.c_str(), str.length()); }

    void println() {
      print(newline.c_str(), newline.length());
    }

    bool println(const char* data, int count) {
        char remap[count + newline.length()];
        remapCharset(data, remap, count);
        remapCharset(newline.c_str(), remap + count, newline.length());

        bool result = iec_send(remap, count + newline.length());

        delay(2);
        return result;
    }

    void println(String str) { println(str.c_str(), str.length()); }

    void setNewline(String newline_char) {
        if (newline_char.length() > 0)
        {
            this->newline = newline_char;
        }
    }

private:
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
    
    void remapCharset(const char* input, char* output, int count)
    {
        for (int i = 0; i < count; i++) {
            output[i] = remapCharset(input[i]);
        }
    }

    bool iec_send(const char* data, int count)
    {
        Serial2.write(0x00);
        Serial2.write((uint8_t)(count >> 8));
        Serial2.write((uint8_t)(count & 0xFF));

        uint8_t temp[3];
        int bytesRead = Serial2.readBytes(temp, 3);
        if (bytesRead != 3) {
            Serial.println("IEC did not accept start command.");
            return false;
        }
        if ((temp[0] != 0x00) || (temp[1] != (count >> 8)) || (temp[2] != (count & 0xFF))) 
        {
            Serial.println("Invalid IEC confirmation returned.");
            return false;
        }

        // Send bytes!
        Serial2.write(data, count);
        
        // Wait for operation complete confirmaiton
        int timeout = 0;
        bool finished = false;
        while (!finished && timeout < 10000)
        {
            delay(100);
            timeout += 100;

            while (Serial2.available() >= 2) {
                if (Serial2.read() == 0)
                {
                    if (Serial2.read() == 0)
                    {
                        finished = true;
                        break;
                    }
                }
            }
        }

        if (!finished)
        {
            Serial.println("Did not receive operation complete confirmation.");
            return false;
        }

        return true;
    }
};

#endif