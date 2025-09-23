#ifndef IEC_DRIVER_H
#define IEC_DRIVER_H

#include <Arduino.h>
#include <cstdint>

// Default timeout waiting for response
#define CBM_TIMEOUT  65000

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

	inline bool read() {
		// The bus line pulled LOW is read as a logical 1 per the IEC bus standard.
		// i.e. the bus is pullsed high in it's default state, and any device on the
		// bus will pull it low to signal a bit.
		return digitalRead(this->pin_input) == LOW;
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


enum class IECResult : int8_t
{
	Failed = -1,
	Success = 0,
	AttentionInterrupt = 1,
	MoreBytes = 2,
	Timeout = 3
};

// Return values for checkATN:
enum class ATNCheck {
	ATN_IDLE = 0,       // Nothing recieved of our concern
	ATN_CMD = 1,        // A command is recieved
	ATN_CMD_LISTEN = 2, // A command is recieved and data is coming to us
	ATN_CMD_TALK = 3,   // A command is recieved and we must talk now
	ATN_ERROR = 4,      // A problem occoured, reset communication
	ATN_RESET = 5				// The IEC bus is in a reset state (RESET line).
};

// IEC ATN commands:
enum class ATNCommand : uint8_t {
	ATN_CODE_LISTEN = 0x20,
	ATN_CODE_TALK = 0x40,
	ATN_CODE_DATA = 0x60,
	ATN_CODE_CLOSE = 0xE0,
	ATN_CODE_OPEN = 0xF0,
	ATN_CODE_UNLISTEN = 0x3F,
	ATN_CODE_UNTALK = 0x5F
};

struct ATNCmd {
	static const uint8_t ATN_CMD_MAX_LENGTH = 40;

	uint8_t code;
	uint8_t str[ATN_CMD_MAX_LENGTH];
	uint8_t strLen;
};

// This class is specifically designed to work on my custom made PCB, so the
// pin assignments are hard-coded.
class IEC
{
public:
	IEC();
	~IEC()
	{ }

	// Initialise IEC driver by setting pin modes and putting bus into the default state.
	void begin();

	// Interrupts the bus (ATN line) to start the transmission of a new command
	void cmd_Start();

	inline IECResult cmd_Global(uint8_t cmd)
	{
		return send(cmd & 0x1F, false, false);
	}

	inline IECResult cmd_Listen(uint8_t addr) 
	{
		return send(0x20 | (addr & 0x1F), false, false);
	}

	// Sets the secondary address, usually used as flags
	inline IECResult cmd_Second(uint8_t addr) 
	{
		return send(0x60 | (addr & 0x1F), false, false);
	}

	inline IECResult cmd_Unlisten() 
	{
		return send(0x3F, false, false);
	}

	inline IECResult cmd_Talk(uint8_t addr) 
	{
		return send(0x40 | (addr & 0x1F), false, false);
	}

	inline IECResult cmd_Untalk() 
	{
		return send(0x5F, false, false);
	}

	inline IECResult cmd_Close(uint8_t addr)
	{
		return send(0xE0 | (addr & 0x1F), false, false);
	}

	inline IECResult cmd_Open(uint8_t addr)
	{
		return send(0xF0 | (addr & 0x1F), false, false);
	}

	// Releases the ATN line at the end of a command transmission
	void cmd_End();

	// Checks if CBM is sending an attention message. If this is the case,
	// the message is recieved and stored in atn_cmd.
	//
	ATNCheck checkATN(ATNCmd& cmd, uint8_t deviceNumber);

	// Checks if CBM is sending a reset (setting the RESET line high). This is typicall
	// when the CBM is reset itself. In this case, we are supposed to reset all states to initial.
	bool checkReset();
	void sendReset();

	// Sends a single byte and can signal EOI
	IECResult send(uint8_t data, bool signalEOI, bool checkATN = true);
	IECResult sendEmptyStream();
	void endTransmission();
	IECResult receive(uint8_t* data, bool checkATN = true);
	IECResult turnAround(bool switchToSender);

private:
	// waitForSignal = if waiting for logical 0 or 1. ex if "waitForSignal=True" then
	// the function will only return when a logical 1 is read from the pin.
	// NOTE: Logical 1 is 0v on the pin, as the IEC bus is flipped.
	IECResult timeoutWait(PinPair* waitBit, bool waitForSignal, bool checkATN = true, uint32_t timeout = CBM_TIMEOUT);

	// input, output pins
	PinPair m_srqPin = PinPair(2, 3);
	PinPair m_atnPin = PinPair(4, 5);
	PinPair m_resetPin = PinPair(6, 7);
	PinPair m_clkPin = PinPair(8, 9);
	PinPair m_dataPin = PinPair(10, 11);
};

#endif