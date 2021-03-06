
#pragma once

#include <inttypes.h>

// radio driver configures HFClock
#include "../hfClock.h"

#include "types.h"	// BufferPointer


/*
 * Low-level driver for radio peripheral
 *
 * Understands registers, etc.
 * Does not understand any wireless protocol.
 * Provides services for wireless protocols.
 *
 * Singleton, all static class methods.
 */
class RadioDevice {

public:
	// Configuration: see notes in radioConfigure.c

	static void configureFixedFrequency(uint8_t frequencyIndex);
	static void configureFixedLogicalAddress();
	static void configureNetworkAddressPool();
	static void configureShortCRC();
	static void configureMediumCRC();
	static void configureStaticPacketFormat(const uint8_t, const uint8_t );
	static void configureWhiteningOn();	// Must follow configureStaticPacketFormat()
	static void configureWhiteningSeed(int);
	static void configureXmitPower(unsigned int dBm);
	static void configureMegaBitrate(unsigned int baud);
	static void configureFastRampUp();
private:
	static void configureShortCRCLength();
	static void configureShortCRCGeneratorInit();
	static void configureShortCRCPolynomialForShortData();
	static void configureStaticOnAirPacketFormat();
	static void configureStaticPayloadFormat(const uint8_t PayloadCount, const uint8_t AddressLength);



	// overloaded
	static void setFirstNetworkAddressInPool(const uint8_t* address, const uint8_t len);
	static void setFirstNetworkAddressInPool();

public:
	static void configurePacketAddress(BufferPointer data);
	static void configurePacketLengthDynamic(uint8_t length);



	// Getters of configuration
	static uint32_t frequency();


	static bool isCRCValid();
	static uint8_t receivedLogicalAddress();

	// Tasks and events
	static void startRXTask();
	static void startTXTask();
#ifdef USE_PACKET_DONE_FOR_EOT
	static bool isPacketDone();
	static void clearPacketDoneEvent();
#else
	// Not implemented or used: static bool isEOTEvent();
	static void clearMsgReceivedEvent();
	static void clearEndTransmitEvent();
#endif

	/*
	 * Events are NOT SAME as state.
	 * !!! DISABLE task sets event and state
	 * DISABLE event and sate set by either DISABLE Task OR after packet done
	 */
	static void startDisablingTask();
	// events
	static bool isDisabledEventSet();
	static void clearDisabledEvent();
	// state remains in effect even after even is cleared
	static bool isDisabledState();

	static bool isReceiveInProgressEvent();
	static void clearReceiveInProgressEvent();

#ifdef USE_PACKET_DONE_FOR_EOT
	static void enableInterruptForPacketDoneEvent();
	static void disableInterruptForPacketDoneEvent();
	static bool isEnabledInterruptForPacketDoneEvent();
#else
	// use disabled event for EOT
	static void enableInterruptForDisabledEvent();
	static void disableInterruptForDisabledEvent();
	static bool isEnabledInterruptForDisabledEvent();
#endif

	static void setShortcutsAvoidSomeEvents();

	static void setRadioPowered(bool);
	static bool isPowerOn();
};
