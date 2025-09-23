#ifndef C64_BASIC_H
#define C64_BASIC_H

#include "iec_driver.h"

class LogicalFile
{
public:
    LogicalFile()
    {
        driver = nullptr;
        device_addr = -1;
        second_addr = -1;
    }

    static bool open(LogicalFile* file_out, IEC* iec_driver, int device_number, int secondary_address = -1)
    {
        iec_driver->cmd_Start();
        
        IECResult result = iec_driver->cmd_Listen((uint8_t)device_number);
        if (result != IECResult::Success) {
            iec_driver->cmd_End();
            Serial.print("Failed to send cmd LISTEN: ");
            Serial.println((int)result);
            return false;
        }

        result = iec_driver->cmd_Second((uint8_t)secondary_address);
        if (result != IECResult::Success) {
            iec_driver->cmd_End();
            Serial.print("Failed to send cmd SECOND: ");
            Serial.println((int)result);
            return false;
        }
        
        iec_driver->cmd_End();

        *file_out = LogicalFile(iec_driver, device_number, secondary_address);
        return true;
    }

    IECResult print(const uint8_t* data, int count, bool EOI = false)
    {
        for (int i = 0; i < count; i++)
        {
            IECResult result = driver->send(data[i], EOI && (i == count - 1));
            if (result != IECResult::Success)
            {
                return result;
            }
        }

        return IECResult::Success;
    }

    IECResult close()
    {
        // Unlisten
        driver->cmd_Start();
        IECResult result = driver->cmd_Unlisten();
        driver->cmd_End();

        return result;
    }

private:
    LogicalFile(IEC* iec_driver, int device_number, int secondary_address)
    {
        this->driver = iec_driver;
        this->device_addr = device_number;
        this->second_addr = secondary_address;
    }

    IEC* driver;
    int device_addr;
    int second_addr;

};

#endif