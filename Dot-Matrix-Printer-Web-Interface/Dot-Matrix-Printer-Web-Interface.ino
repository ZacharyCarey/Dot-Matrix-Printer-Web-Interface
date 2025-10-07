//#include <WiFi.h>
//#include "ESPAsyncWebServer.h"
//#include "WebSocketsServer.h"

#include "okidata_120.h"

// Mapping of CBM-64's serial port lines to Arduino's digital I/O pins.
// With Brother HR-5C, we need to be able to read only the data line.
Okidata120 cbm;

// For testing:
int TEST_MODE = -1;
const int DATA_MAX_LENGTH = 1000;
char data[DATA_MAX_LENGTH];
int data_index = -1;

void printSelfTest() {
  cbm.println("Hello World, here are the chars supported by the printer:");
  cbm.println();
  const uint32_t count = 16*26;
  char data[count];
  for (int i=0; i<16; i++)
  {
    int index = 24*i;
    data[index] = i+32;
    data[index+1] = ' ';
    data[index+2] = i+48;
    data[index+3] = ' ';
    data[index+4] = i+64;
    data[index+5] = ' ';
    data[index+6] = i+80;
    data[index+7] = ' ';
    data[index+8] = i+96;
    data[index+9] = ' ';
    data[index+10] = i+112;
    data[index+11] = ' ';
    data[index+12] = i+160;
    data[index+13] = ' ';
    data[index+14] = i+176;
    data[index+15] = ' ';
    data[index+16] = i+192;
    data[index+17] = ' ';
    data[index+18] = i+200;
    data[index+19] = ' ';
    data[index+20] = i+224;
    data[index+21] = ' ';
    data[index+22] = i+240;
    data[index+23] = '\r';
  }
  cbm.println(data, count);
}

// For testing, print menu to computer.
void test_menu()
{
  Serial.println("Select printer test mode:");
  Serial.println(" (1) Print self test (charset)");
  Serial.println(" (2) Print user message");
  Serial.println(" (3) Release printer");
  Serial.print("Your selection: > ");
}

// Arduino setup function is run once when the sketch starts.
void setup()
{
  Serial.begin(115200);

  Serial.println("Initializing printer...");
  cbm.begin(PrintMode::Graphic);
  //cbm.setNewline("\r\n");
  Serial.println("ready!");

  // Print test menu.
  /*for (int i = 0; i < 3; i++) {
    cbm.println("Hello, world!");
  }*/
  cbm.println("Line 1");
  cbm.println("Line 2");
  cbm.println("Line 3");

  test_menu();
}

// Arduino loop function is run over and over again, forever.
void loop()
{
  char val;

  // Check if data has been sent from the computer.
  if (Serial.available())
  {
    // Read the most recent byte (which will be from 0 to 255).
    val = Serial.read();
    Serial.println(val);

    if (TEST_MODE <= 0)
      { // Set test mode
      if (val == '1') TEST_MODE = 1;
      else if (val == '2') TEST_MODE = 2;
      }

    if (TEST_MODE == 1)
    { // If self test mode selected, do self test now.
      Serial.println("Now printing...");
      printSelfTest();
      Serial.println("Done.");
      TEST_MODE = 0;
      test_menu();
    }
    else if (TEST_MODE == 2)
    {
      if (data_index == -1)
      { // If user message test mode selected, ask it now.
        Serial.println("Type text to be printed (# ends):");
        data_index++;
      }
      else
      {
        if ( val == '#' )
        {
          data[data_index] = '\0';
          cbm.println(data);
          Serial.println("Done.");
          data_index = -1;
          TEST_MODE = 0;
          test_menu();
        }
        else if (data_index < DATA_MAX_LENGTH)
        {
          data[data_index] = val;
          data_index++;
          if (data_index + 1 >= DATA_MAX_LENGTH)
          {
            data[data_index] = '\0';
            cbm.println(data, DATA_MAX_LENGTH);
            data_index = 0;
          }
        }
      }
    }
  }
}