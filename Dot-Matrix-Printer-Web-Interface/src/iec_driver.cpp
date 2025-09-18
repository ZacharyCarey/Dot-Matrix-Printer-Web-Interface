#include "iec_driver.h"
//#include "log.h"

/******************************************************************************
 *                                                                             *
 *                                TIMING SETUP                                 *
 *                                                                             *
 ******************************************************************************/


// IEC protocol timing consts:
#define TIMING_BIT          70  // bit clock hi/lo time     (us)
#define TIMING_STABLE_WAIT  20  // line stabilization       (us)
#define TIMING_BETWEEN_BYTES 100 // delay time between bytes to allow receiver to signal busy (us)
#define TIMING_EOI_WAIT     200 // delay to signal EOI      (us)
#define TIMING_EMPTY_STREAM     500 // delay to signal empty stream      (us)
#define TIMING_EOI_THRESH   19  // threshold for EOI detect (*10 us approx)
#define TIMING_ATN_START 1000 // Time attention is pulled low waiting for listeners (us)

#define TIMING_ATN_PREDELAY 50  // delay required in atn    (us)
#define TIMING_ATN_DELAY    100 // delay required after atn (us)
#define TIMING_FNF_DELAY    100 // delay after fnf?         (us)

// Version 0.5 equivalent timings: 70, 5, 200, 20, 20, 50, 100, 100

// TIMING TESTING:
//
// The consts: 70,20,200,20,20,50,100,100 has been tested without debug print
// to work stable on my (Larsp)'s DTV at 700000 < F_CPU < 9000000
// using a 32 MB MMC card
//

IEC::IEC()
{
	
} // ctor

// Set all IEC_signal lines in the correct mode
//
void IEC::begin()
{
	// make sure the output states are initially LOW.
	this->m_srqPin.begin();
	this->m_atnPin.begin();
	this->m_resetPin.begin();
	this->m_clkPin.begin();
	this->m_dataPin.begin();
}


IECResult IEC::timeoutWait(PinPair* waitBit, bool waitForSignal, bool checkATN, uint32_t timeout)
{
	uint32_t time = 0;
	bool state;

	while(time < timeout) {
		// Check for Attention inerrupt
		if (checkATN && m_atnPin.read())
		{
			return IECResult::AttentionInterrupt;
		}
		
		// Check the waiting condition:
		state = waitBit->read();

		if (state == waitForSignal) {
			return IECResult::Success;
		}

		delayMicroseconds(2); // The aim is to make the loop at least 3 us
		time += 2;
	}

	return IECResult::Failed;
}

// Assumptions:
//	Sender (us) is holding CLK logic 1 to indicate not yet ready to send.
//	Some minimum amount of time has passed to allow listeners to pull DATA high.
// If signalEOI is false, it is assumed this function will be called again for another byte.
IECResult IEC::sendByte(uint8_t data, bool signalEOI, bool checkATN)
{
	// Indicate we are ready to send data
	this->m_clkPin.write(false);

	// Wait for listener to be ready
	IECResult result = timeoutWait(&m_dataPin, false, checkATN);
	if(result != IECResult::Success)
		return result;

	if(signalEOI) {
		// Signal eoi by waiting 200 us
		delayMicroseconds(TIMING_EOI_WAIT);

		// get eoi acknowledge
		result = timeoutWait(&m_dataPin, true, checkATN);
		if(result != IECResult::Success)
			return result;

		this->m_clkPin.write(true);

		// IEC dictates this must be at least 60us, but we aren't
		// going to attempt timing that on an arduino (for simplicity).
		// Just assume the receiver conforms to the standard.
		result = timeoutWait(&m_dataPin, false, checkATN);
		if(result != IECResult::Success)
			return result;
	}

	// Small delay before starting transmission
	delayMicroseconds(TIMING_STABLE_WAIT);

	// Send bits
	for(int n = 0; n < 8; n++) {
		// Clear clock
		this->m_clkPin.write(true);
		delayMicroseconds(TIMING_BIT / 2);

		// set data
		this->m_dataPin.write((data & 1) ? false : true); // Still not 100% sure why this is flipped.
		delayMicroseconds(TIMING_BIT / 2);

		// set clock
		this->m_clkPin.write(false);
		delayMicroseconds(TIMING_BIT);

		// Check for Attention inerrupt
		if (checkATN && m_atnPin.read())
		{
			return IECResult::AttentionInterrupt;
		}

		data >>= 1;
	}

	// End transmission
	this->m_clkPin.write(true); // hold for next byte
	this->m_dataPin.write(false);

	// Minimum time between multiple bytes
	delayMicroseconds(/*TIMING_BETWEEN_BYTES*/20);

	// Wait for receivers to acknowledge data
	timeoutWait(&m_dataPin, true, checkATN);

	delayMicroseconds(TIMING_BETWEEN_BYTES);

	// Check for Attention inerrupt
	if (checkATN && m_atnPin.read())
	{
		return IECResult::AttentionInterrupt;
	}

	// during this time receivers are allowed to indicate they are busy
	// We will wait for them to be ready the next time this function is called

	return IECResult::Success;
}

// Special empty stream case which sends no bytes and send EOI immediately
IECResult IEC::sendEmptyStream()
{
	// Indicate we are ready to send data
	this->m_clkPin.write(false);

	// Wait for listener to be ready
	IECResult result = timeoutWait(&m_dataPin, false);
	if(result != IECResult::Success)
		return result;

	// Small delay before starting transmission
	delayMicroseconds(TIMING_STABLE_WAIT);

	// Leave bus inactive to signal early EOI
	this->m_clkPin.write(false);
	this->m_dataPin.write(false);

	return IECResult::Success;
}

void IEC::endTransmission()
{
	this->m_clkPin.write(false);
}

IECResult IEC::receiveByte(uint8_t* data, bool checkATN)
{
	// Indicate we are ready to read
	this->m_dataPin.write(false);

	// Wait for talker ready
	IECResult result = timeoutWait(&m_clkPin, false, checkATN);
	if(result != IECResult::Success)
		return result;

	// Record how long CLOCK is false, more than 200 us means EOI
	uint8_t n = 0;
	while(this->m_clkPin.read() == false && (n < 30)) {
		delayMicroseconds(10);  // this loop should cycle in about 10 us...
		n++;
	}

	if (checkATN && this->m_atnPin.read()) 
	{
		return IECResult::AttentionInterrupt;
	}

	// Clock never came back but it wasnt an interrupt
	if (n == 30) {
		return IECResult::Failed;
	}

	bool lastByte = false;
	if(n >= TIMING_EOI_THRESH) {
		// Acknowledge by pull down data more than 60 us
		this->m_dataPin.write(true);
		delayMicroseconds(TIMING_BIT);
		this->m_dataPin.write(false);

		// but still wait for clk
		//if(timeoutWait(&m_clkPin, true))
		//	return 0;

		lastByte = true;
	}

	*data = 0;
	// Get the bits, sampling on clock rising edge:
	for(n = 0; n < 8; n++) {
		(*data) >>= 1;

		// Wait for sender to indicate invalid data
		result = timeoutWait(&m_clkPin, true, checkATN);
		if(result != IECResult::Success)
			return result;

		// Wait for sender to indicate next data bit
		result = timeoutWait(&m_clkPin, false, checkATN);
		if(result != IECResult::Success)
			return result;

		// Save bit
		(*data) |= (this->m_dataPin.read() ? (1 << 7) : 0);
	}

	// Wait for the sender to finish the byte transmission
	result = timeoutWait(&m_clkPin, true, checkATN);
	if (result != IECResult::Success)
		return result;

	// Signal we are busy:
	this->m_dataPin.write(true);

	if (lastByte) {
		return IECResult::Success;
	} else {
		return IECResult::MoreBytes;
	}
} // receiveByte

void IEC::cmd_Start()
{
	// Header begins, write ACK
	this->m_atnPin.write(true);
	delayMicroseconds(/*TIMING_ATN_START*/2000);
	this->m_clkPin.write(true);
	delayMicroseconds(/*TIMING_ATN_START*/2000);
}

void IEC::cmd_End()
{
	delayMicroseconds(TIMING_STABLE_WAIT);
	this->m_atnPin.write(false);
	//delayMicroseconds(TIMING_STABLE_WAIT);
}

// IEC turnaround. Switch from us sending to other device sending
IECResult IEC::turnAround(bool switchToSender)
{
	if (switchToSender)
	{
		// Wait until clock is released
		IECResult result = timeoutWait(&m_clkPin, false);
		if(result != IECResult::Success)
			return result;

		delayMicroseconds(TIMING_STABLE_WAIT);
		this->m_dataPin.write(false);
		this->m_clkPin.write(true);
		delayMicroseconds(TIMING_BIT);
		
		return IECResult::Success;
	}
	else
	{
		this->m_dataPin.write(true);
		this->m_clkPin.write(false);
		
		// Wait for the new sender to pull down the clock
		delayMicroseconds(TIMING_STABLE_WAIT);
		IECResult result = timeoutWait(&m_clkPin, true);
		if (result != IECResult::Success)
			return result;

		return IECResult::Success;
	}
}


/******************************************************************************
 *                                                                             *
 *                               Public functions                              *
 *                                                                             *
 ******************************************************************************/

// This function checks and deals with atn signal commands
//
// If a command is recieved, the cmd-string is saved in cmd. Only commands
// for *this* device are dealt with.
//
// Return value, see IEC::ATNCheck definition.
ATNCheck IEC::checkATN(ATNCmd& cmd, uint8_t m_deviceNumber)
{
//	if (!this->m_atnPin.read())
//	{
		return ATNCheck::ATN_IDLE;
//	}

/*	// Attention line is active, go to listener mode and get message. Being fast with the next two lines here is CRITICAL!
	this->m_dataPin.write(true);
	this->m_clkPin.write(false);
	delayMicroseconds(TIMING_ATN_PREDELAY);

	// Get first ATN byte, it is either LISTEN or TALK
	ATNCommand c;
	if(receiveByte((uint8_t*)&c) != IECResult::Success)
		return ATNCheck::ATN_ERROR;

	if((uint8_t)c == ((uint8_t)ATNCommand::ATN_CODE_LISTEN bitor m_deviceNumber)) {
		// Okay, we will listen.
		// Get the first cmd byte, the cmd code
		if (receiveByte((uint8_t*)&c) != IECResult::Success)
			return ATNCheck::ATN_ERROR;

		cmd.code = (uint8_t)c;

		// If the command is DATA and it is not to expect just a small command on the command channel, then
		// we're into something more heavy. Otherwise read it all out right here until UNLISTEN is received.
		if(((uint8_t)c bitand 0xF0) == (uint8_t)ATNCommand::ATN_CODE_DATA and ((uint8_t)c bitand 0xF) not_eq CMD_CHANNEL) {
			// A heapload of data might come now, too big for this context to handle so the caller handles this, we're done here.
			//Log(Information, FAC_IEC, "LDATA");
			ret = ATNCheck::ATN_CMD_LISTEN;
		}
		else if(c not_eq ATNCommand::ATN_CODE_UNLISTEN) {
			// Some other command. Record the cmd string until UNLISTEN is sent
			for(;;) {
				if(receiveByte((uint8_t*)&c) != IECResult::Success)
					return ATNCheck::ATN_ERROR;

				if((m_state bitand atnFlag) and (ATNCommand::ATN_CODE_UNLISTEN == c))
					break;

				if(i >= ATN_CMD_MAX_LENGTH) {
					// Buffer is going to overflow, this is an error condition
					// FIXME: here we should propagate the error type being overflow so that reading error channel can give right code out.
					return ATNCheck::ATN_ERROR;
				}
				cmd.str[i++] = c;
			}
			ret = ATNCheck::ATN_CMD;
		}
	}
	else if ((uint8_t)c == ((uint8_t)ATNCommand::ATN_CODE_TALK bitor m_deviceNumber)) {
		// Okay, we will talk soon, record cmd string while ATN is active
		// First byte is cmd code, that we CAN at least expect. All else depends on ATN.
		if(receiveByte((uint8_t*)&c) != IECResult::Success)
			return ATNCheck::ATN_ERROR;
		cmd.code = c;

		while(!this->m_atnPin.read()) {
			if(this->m_clkPin.read()) {
				if(receiveByte((uint8_t*)&c) != IECResult::Success)
					return ATNCheck::ATN_ERROR;

				if(i >= ATN_CMD_MAX_LENGTH) {
					// Buffer is going to overflow, this is an error condition
					// FIXME: here we should propagate the error type being overflow so that reading error channel can give right code out.
					return ATNCheck::ATN_ERROR;
				}
				cmd.str[i++] = c;
			}
		}

		// Now ATN has just been released, do bus turnaround
		if(not turnAround())
			return ATNCheck::ATN_ERROR;

		// We have recieved a CMD and we should talk now:
		ret = ATNCheck::ATN_CMD_TALK;

	}
	else {
		// Either the message is not for us or insignificant, like unlisten.
		delayMicroseconds(TIMING_ATN_DELAY);
		this->m_dataPin.write(false);
		this->m_clkPin.write(false);
		//			{
		//				char buffer[20];
		//				sprintf(buffer, "NOTUS: %u", c);
		//				Log(Information, FAC_IEC, buffer);
		//			}

		// Wait for ATN to release and quit
		while(!this->m_atnPin.read());
		//Log(Information, FAC_IEC, "ATNREL");
	}

	// some delay is required before more ATN business can take place.
	delayMicroseconds(TIMING_ATN_DELAY);

	cmd.strLen = i;
	return ret;*/
} // checkATN


bool IEC::checkReset()
{
	return this->m_resetPin.read();
}

void IEC::sendReset() {
	this->m_resetPin.write(true);
	delay(100);
	this->m_resetPin.write(false);
	delay(3000); // give time for device to reset
}