#ifndef IEC_DRIVER_H
#define IEC_DRIVER_H

#include <Arduino.h>
#include "global_defines.h"
#include "cbmdefines.h"

class PinPair 
{
public:
	PinPair(int input, int output) 
	{
		this->pin_input = input;
		this->pin_output = output;
	}

	void begin() 
	{
		pinMode(this->pin_input, INPUT);
		pinMode(this->pin_output, OUTPUT);
		digitalWrite(this->pin_output, LOW);
	}

	// FALSE = float bus high
	// TRUE = pull bus low
	inline void write(bool state) {
		digitalWrite(this->pin_output, state);
	}

	inline int read() {
		digitalRead(this->pin_input);
	}

private:
	PinPair();

	// Pin used to control the data line
	// Writing LOW lets the bus float HIGH
	// Writing HIGH pulls the bus LOW
	int pin_output;

	// Pin used to read the state of the bus
	// Read the real state, LOW or HIGH
	int pin_input;
};

class IEC
{
public:

	enum IECState {
		noFlags   = 0,
		eoiFlag   = (1 << 0),   // might be set by Iec_receive
		atnFlag   = (1 << 1),   // might be set by Iec_receive
		errorFlag = (1 << 2)  // If this flag is set, something went wrong and
	};

	// Return values for checkATN:
	enum ATNCheck {
		ATN_IDLE = 0,       // Nothing recieved of our concern
		ATN_CMD = 1,        // A command is recieved
		ATN_CMD_LISTEN = 2, // A command is recieved and data is coming to us
		ATN_CMD_TALK = 3,   // A command is recieved and we must talk now
		ATN_ERROR = 4,      // A problem occoured, reset communication
		ATN_RESET = 5				// The IEC bus is in a reset state (RESET line).
	};

	// IEC ATN commands:
	enum ATNCommand {
		ATN_CODE_LISTEN = 0x20,
		ATN_CODE_TALK = 0x40,
		ATN_CODE_DATA = 0x60,
		ATN_CODE_CLOSE = 0xE0,
		ATN_CODE_OPEN = 0xF0,
		ATN_CODE_UNLISTEN = 0x3F,
		ATN_CODE_UNTALK = 0x5F
	};

	// ATN command struct maximum command length:
	enum {
		ATN_CMD_MAX_LENGTH = 40
	};

	typedef struct _tagATNCMD {
		byte code;
		byte str[ATN_CMD_MAX_LENGTH];
		byte strLen;
	} ATNCmd;

	IEC();
	~IEC()
	{ }

	// Initialise iec driver
	//
	boolean begin(byte deviceNumber);

	// Checks if CBM is sending an attention message. If this is the case,
	// the message is recieved and stored in atn_cmd.
	//
	ATNCheck checkATN(ATNCmd& cmd);

	// Checks if CBM is sending a reset (setting the RESET line high). This is typicall
	// when the CBM is reset itself. In this case, we are supposed to reset all states to initial.
	boolean checkRESET();

	boolean sendReset() {
		this->m_resetPin.write(true);
		delay(100);
		this->m_resetPin.write(false);
		delay(3000); // give time for device to reset
	}

	bool sendHeader(byte mode) {
		// Header begins, write ACK
		this->m_atnPin.write(true);
		delayMicroseconds(2000);
		this->m_clockPin.write(true);
		delayMicroseconds(2000);

		// Write listener address
		send((0x20 + m_deviceNumber), false);

		// Write secondary address
		send((0x60 + mode), false);

		// End header
		delayMicroseconds(20);
		this->m_atnPin.write(false);
	}

	// Sends a byte. The communication must be in the correct state: a load command
	// must just have been recieved. If something is not OK, FALSE is returned.
	//
	boolean send(byte data, boolean isLastByte);

	// A special send command that informs file not found condition
	//
	boolean sendFNF();

	// Recieves a byte
	//
	byte receive();

	byte deviceNumber() const;
	IECState state() const;

#ifdef DEBUGLINES
	unsigned long m_lastMillis;
	void testINPUTS();
	void testOUTPUTS();
#endif

private:
	byte timeoutWait(PinPair* waitBit, boolean whileHigh);
	byte receiveByte(void);
	boolean sendByte(byte data, boolean signalEOI);
	boolean turnAround(void);
	boolean undoTurnAround(void);

	// communication must be reset
	byte m_state;
	byte m_deviceNumber;

	// input, output pins
	PinPair m_srqInPin = PinPair(2, 3);
	PinPair m_atnPin = PinPair(4, 5);
	PinPair m_resetPin = PinPair(6, 7);
	PinPair m_clockPin = PinPair(8, 9);
	PinPair m_dataPin = PinPair(10, 11);
};

#endif