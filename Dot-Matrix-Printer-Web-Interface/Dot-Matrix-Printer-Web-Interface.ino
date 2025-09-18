//#include <WiFi.h>
//#include "ESPAsyncWebServer.h"
//#include "WebSocketsServer.h"

#include "okidata_120.h"

// Mapping of CBM-64's serial port lines to Arduino's digital I/O pins.
// With Brother HR-5C, we need to be able to read only the data line.
Okidata120 cbm;
/*
// For testing:
int TEST_MODE = -1;
const int DATA_MAX_LENGTH = 1000;
char data[DATA_MAX_LENGTH];
int data_index = -1;

void printSelfTest() {
  cbm.println("Hello World, here are the chars supported by the printer:");
  cbm.println();
  for (int i=0; i<16; i++)
  {
    cbm.print(i+32);
    cbm.print(' ');
    cbm.print(i+48);
    cbm.print(' ');
    cbm.print(i+64);
    cbm.print(' ');
    cbm.print(i+80);
    cbm.print(' ');
    cbm.print(i+96);
    cbm.print(' ');
    cbm.print(i+112);
    cbm.print("    ");
    cbm.print(i+160);
    cbm.print(' ');
    cbm.print(i+176);
    cbm.print(' ');
    cbm.print(i+192);
    cbm.print(' ');
    cbm.print(i+200);
    cbm.print(' ');
    cbm.print(i+224);
    cbm.print(' ');
    cbm.print(i+240);
    cbm.println();
  }
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
*/
// Arduino setup function is run once when the sketch starts.
void setup()
{
  Serial.begin(115200);

  Serial.println("Initializing printer...");
  cbm.begin(PrintMode::Graphic);
  //cbm.setNewline("\r\n");
  Serial.println("ready!");

  // Print test menu.
  //cbm.println("Hello, world!");
  //test_menu();
}

// Arduino loop function is run over and over again, forever.
void loop()
{
/*  char val;

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
      else if (val == '3') TEST_MODE = 3;
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
    else if (TEST_MODE == 3)
    {
      cbm.end();
      Serial.println("Printer has been released.");
      while(true){}
    }
  }*/
}