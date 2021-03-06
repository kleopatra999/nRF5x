
#include "nvic.h"

#include "nrf.h"

// Implementation uses macros defined in /components/toolchain/cmsis/include/ e.g. core_cm4.h

void Nvic::enableRadioIRQ() {
	NVIC_ClearPendingIRQ(RADIO_IRQn);
	NVIC_EnableIRQ(RADIO_IRQn);
}

void Nvic::disableRadioIRQ() {
	NVIC_ClearPendingIRQ(RADIO_IRQn);
	NVIC_DisableIRQ(RADIO_IRQn);
}

void Nvic::enableRTC0IRQ() {
	NVIC_ClearPendingIRQ(RTC0_IRQn);
	NVIC_EnableIRQ(RTC0_IRQn);
}

void Nvic::enablePowerClockIRQ(){
	NVIC_ClearPendingIRQ(POWER_CLOCK_IRQn);
	NVIC_EnableIRQ(POWER_CLOCK_IRQn);
}

void Nvic::disablePowerClockIRQ(){
	NVIC_ClearPendingIRQ(POWER_CLOCK_IRQn);
	NVIC_DisableIRQ(POWER_CLOCK_IRQn);
}

