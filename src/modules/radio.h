#pragma once

#include <inttypes.h>

#include "../drivers/hfClock.h"
#include "../drivers/nvic.h"
#include "../drivers/powerSupply.h"

#include "../drivers/radio/types.h"	// BufferPointer


typedef enum {
	Receiving,
	Transmitting,
	Idle,
	PowerOff
} RadioState;


/*
 * High level driver for radio peripheral
 *
 * Understands collaboration of low-level devices:
 * - HfClock and RadioDevice
 * - DCDCPower and RadioDevice
 *
 * Not understand registers, i.e no dependence on nrf.h
 *
 * Understands wireless protocol:
 * - configures device for protocol
 * - abstracts protocol state from device state
 * - abstracts away the task/event architecture of device
 * - implements interrupt RX vs. spinning TX
 * - understands half-duplex (RX exclusive of TX)
 *
 * Protocol is defined by:
 * - constants for lengths, channels, bitrate
 * - behaviour
 * -- no acks xmitted as in ESB
 * -- all units use one address
 *
 * Singleton, all static class methods.
 */
/*
 * Algebra of valid call sequences:
 *
 *  Typical:
 *    init(), powerOnAndConfigure(), getBufferAddress(), <put payload in buffer> transmitStaticSynchronously(),
 *            receiveStatic(), sleepUntilEventWithTimeout(),  if isDisabledState() getBufferAddress(); <get payload from buffer>,
 *            powerOff(), powerOnAndConfigure(), ...
 *
 *  init() must be called once after mcu POR:
 *    mcu.reset(), init(), powerOnAndConfigure(), ...., mcu.reset(), init(), ...
 *
 *  configure() must be called after powerOn() (use convenience function powerOnAndConfigure())
 *    init(), powerOn(), configureStatic(), receiveStatic, ...
 *
 *  transmitStaticSynchronously blocks, but receiveStatic does not.  If reasonForWake is not MsgReceived,
 *  you must stopReceive() before next radio operation:
 *    receiveStatic(), sleepUntilEventWithTimeout(),
 *      if sleeper.reasonForWake() != MsgReceived then stopReceive()
 *
 *  When reasonForWake is MsgReceived, buffer is full and radio is ready for next transmit or receive.
 *  A packet received can have an invalidCRC.
 *  A packet received having validCRC can be garbled (when count of bit errors greater than CRC can detect.)
 *     receivedStatic(), sleepUntilEventWithTimeout(), if sleeper.reasonForWake() == MsgReceived then
 *        getBuffer(), if isPacketCRCValid() then <use received buffer>,
 *        transmitStaticSynchronously()
 *
 *  You can transmit or receive in any order, but Radio is half-duplex:
 *    init(), powerOnAndConfigure(), receiveStatic(), ...stopReceive(),
 *       receiveStatic(), ..., stopReceive
 *       transmitStaticSynchronously(), ...
 *
 *	Radio isDisabledState() is true after certain other operations:
 *	   powerOnAndConfigure(), assert(isDisabledState())
 *	   transmitStaticSynchronously(), assert(isDisabledState())
 *	   stopReceive(), assert(isDisabledState())
 *	   receiveStatic(), sleepUntilEventWithTimeout(), if sleeper.reasonForWake() == MsgReceived assert(isDisabledState())
 *
 *	Radio isDisabledState() is false at least for a short time after a receive()
 *	but if a packet is received, is true
 *	    receiveStatic(), assert(! isDisabledState()), ..<packet received>, assert(isDisabledState())
 *
 *  Radio is has a single buffer, non-queuing.  After consecutive operations, first contents of buffer are overwritten.
 */
class Radio {

public:
	// Define protocol lengths

	/*
	 * Fixed: all payloads same size.
	 *
	 * Device configured to not transmit S0, LENGTH, S1
	 * Buffer not include S0, LENGTH, S1 fields.
	 *
	 * Must match length of Message class (struct).
	 * Which for SleepSync is 1 MessageType + 6 MasterID + 3 offset + 1 WorkPayload
	 */
	static const uint8_t FixedPayloadCount = 11;

	/*
	 * bitrate in megabits [1,2]
	 */
	static const uint8_t MegabitRate = 2;


	// Length of transmitted physical layer address (not part of payload.)
	// All units have same physical address so
	static const uint8_t LongNetworkAddressLength = 4;	// 1 byte preamble, 3 bytes base
	static const uint8_t MediumNetworkAddressLength = 3;	// 1 byte preamble, 2 bytes base
	static const uint8_t ShortNetworkAddressLength = 2;	// 1 byte preamble, 1 bytes base

	/*
	 * Frequency index in [0..100]
	 *
	 * Freq fixed to one of 3 BT advertising channels, to avoid WiFi interference.
	 *
	 * 2, 26, 80 : freq = 2.4gHz + FrequencyIndex
	 * e.g. 80 yields 2480 kHz
	 *
	 * The BT channel ID's are much different.
	 * BT is 2402 to 2480
	 *
	 * TODO why not use above 2480?
	 */
	static const uint8_t FrequencyIndex = 80;


	// Radio knows and exposes to others
	static HfCrystalClock* hfCrystalClock;


	static void receivedEventHandler();

	/*
	 * Tells radio of needed other devices. More configuration required.
	 */
	static void init(
			// Used devices
			Nvic*,
			PowerSupply*,
			HfCrystalClock*
			);

	/*
	 * Set callback for all physical messages received.
	 * Callback is usually to another protocol layer, not necessarily to app layer.
	 */
	static void setMsgReceivedCallback(void (*onRcvMsgCallback)());


	/*
	 * Configure parameters of physical protocol: freq, addr, CRC, bitrate, etc
	 */
	static void configurePhysicalProtocol();

	// platform independent 1: +4, 8: -40, else 0.   Units dBm.
	// FUTURE enum
	static void configureXmitPower(unsigned int dBm);

	static void powerOnAndConfigure();
	static void powerOff();
	static bool isPowerOn();

	static bool isDisabledState();
	static bool isEnabledInterruptForPacketDoneEvent();

	// FUTURE DYNAMIC static void getBufferAddressAndLength(uint8_t** handle, uint8_t* lengthPtr);
	// Can't define in-line, is exported
	static BufferPointer getBufferAddress();


	// Static: buffer owned by radio, of fixed length
	static void transmitStatic();
	static void spinUntilXmitComplete();
	static void transmitStaticSynchronously();
	static void stopXmit();

	static void receiveStatic();
	// Only returns true once, until after startReceive again.
	static bool isReceiveInProgress();
	static void clearReceiveInProgress();
	static void spinUntilReceiveComplete();
	static void stopReceive();

	static bool isPacketCRCValid();

	static bool isEnabledInterruptForMsgReceived();
	static bool isEnabledInterruptForEndTransmit();

#ifdef DYNAMIC
	static void transmit(BufferPointer data, uint8_t length);
	static void transmitSynchronously(BufferPointer data, uint8_t length);
	static void receive(BufferPointer data, uint8_t length);

	static void setupXmitOrRcv(BufferPointer data, uint8_t length);
#endif





// FUTURE to anon namespace
private:
	static void setupFixedDMA();

	static void powerOn();	// public uses powerOnAndConfigure, a radio on is useless without configuration
	static void spinUntilReady();
	static void dispatchPacketCallback();

	static void disable();
	static void spinUntilDisabled();
	static void setupInterruptForMsgReceivedEvent();

	static void startXmit();
	static void startRcv();

	static void enableRXTask();
	static void enableTXTask();

	static bool isEventForMsgReceivedInterrupt();
	static void clearEventForMsgReceivedInterrupt();
	static void enableInterruptForMsgReceived();
	static void disableInterruptForMsgReceived();
	static void disableInterruptForEndTransmit();
};
