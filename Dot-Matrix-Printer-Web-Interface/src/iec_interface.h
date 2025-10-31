#ifndef IEC_INTERFACE_H
#define IEC_INTERFACE_H

#include "arduino.h"

#define BUSY_PIN A4
#define ERROR_WAIT_TIME 200
#define TIMEOUT_TIME 50

void iec_init()
{
    Serial2.begin(115200, SERIAL_8N1, 19, 20); //rx=19(A2), tx=20(A3)
    Serial2.setTimeout(TIMEOUT_TIME);

    // Try to clear out some bad chars
    for (int i = 0; i < 10; i++)
    {
        Serial2.write(0x00);
    }
}

// Data MUST NOT contain any zeros
const char* iec_send(uint8_t* data, uint32_t len)
{
    if (digitalRead(BUSY_PIN) == true)
    {
        return "Printer busy.";
    }

    // Send bytes!
    Serial2.write(data, len);
    Serial2.write(0x00);

    // Wait for busy signal
    int t = 0;
    while ((t < 500) && digitalRead(BUSY_PIN) == false)
    {
        delayMicroseconds(1000);
        t++;
    }
    if (t == 500)
    {
        return "Never received busy signal";
    }

    return nullptr;
}

#endif