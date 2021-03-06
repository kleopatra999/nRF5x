
#include "powerManager.h"
#include "../drivers/powerComparator.h"

namespace {

/*
 * Somewhat arbitrary choice to use this device: simpler and portable.
 *
 * You could use ADC or SAADC or nrf52 whose PowerComparator defines more levels.
 * Then you could define whatever levels you wish.
 */
PowerComparator powerComparator;

}  // namespace



/*
 * Specific to my application:
 *
 * Given:
 *  - storage capacitor is certain joules.
 *  - power supply delivers about 10uA @2.4V (solar cell, in 200 lux.)
 *  - power supply can deliver 4.8V (solar cell in full sun)
 *  - mcu Vbrownout (aka Vmin) is 1.9V
 *  - mcu Vmax is 3.6V
 *
 * Need say 2.1V on capacitor to burst the radio without brownout.
 * Need say 2.3V on capacitor to burst the work without falling below 2.1V
 *
 * Also, these levels are somewhat arbitrary, convenient to implement using PowerComparator.
 *
 */

/*
 * Levels
 *
 * Four of them, constrained by what device implements.
 *
 * Here we also attach meaning, used by SleepSync algorithm/application.
 *
 * You can swap the middle meanings.
 * Currently the firefly algorithm uses most current for work, so reserve is below that.
 *
 * Brownout is 1.8V.
 * External voltage monitor resets at 2.0V.
 * Boot requires a small reserve.
 * Radio requires about 0.2V on capacitor.
 * Radio must not brownout mcu.
 * Work requires > 0.2V on capacitor.
 * Work may dip voltage below that needed for radio.
 * Radio is required to keep sync.
 * Work may temporarily take Vcc below needed for radio.
 */
bool PowerManager::isPowerExcess()     { return powerComparator.isVddGreaterThan2_7V();}
bool PowerManager::isPowerForWork()    { return powerComparator.isVddGreaterThan2_5V();}
bool PowerManager::isPowerForRadio()   { return powerComparator.isVddGreaterThan2_3V();}
bool PowerManager::isPowerForReserve() {return powerComparator.isVddGreaterThan2_1V();}


VoltageRange PowerManager::getVoltageRange() {
	VoltageRange result;
	if (powerComparator.isVddGreaterThan2_7V()) {
		result = VoltageRange::Excess;
	}
	else if (powerComparator.isVddGreaterThan2_5V()) {
		result = VoltageRange::High;
	}
	else if (powerComparator.isVddGreaterThan2_3V()) {
		result = VoltageRange::Medium;
	}
	else if (powerComparator.isVddGreaterThan2_1V()) {
		result = VoltageRange::Low;
	}
	else {
		// < 2.1V, near brownout of say 1.8V
		result = VoltageRange::UltraLow;
	}

	return result;
}

#ifdef OLD
bool PowerManager::isVoltageExcess() { return ;}

bool PowerManager::isVoltageHigh() {
	return ! powerComparator.isVddGreaterThan2_7V()
			& powerComparator.isVddGreaterThan2_5V();
}
bool PowerManager::isVoltageMedium() {
	return ! powerComparator.isVddGreaterThan2_5V()
			& powerComparator.isVddGreaterThan2_3V();
}
bool PowerManager::isVoltageLow() {
	return ! powerComparator.isVddGreaterThan2_3V()
			& powerComparator.isVddGreaterThan2_1V();
}
bool PowerManager::isVoltageUltraLow() {

	return ! powerComparator.isVddGreaterThan2_1V();
}
#endif



