
#include <inttypes.h>
#include "nvic.h"


/*
 * Type the Counter yields.  Only 24 bits are valid.
 */
typedef uint32_t OSTime;

class Counter {
public:
	static void init(Nvic*);
	static void start();
	static void stop();

	static void configureOverflowInterrupt();
	static void clearOverflowEvent();
	static bool isOverflowEvent();
#ifdef OLD
	static void enableCompareReg0Interrupt();
	static void clearCompareReg0Event();
	static bool isCompareReg0Event();

	static void setCompareReg0(unsigned int);
	static void disableCompareReg0Interrupt();
#endif
	static OSTime ticks();
};
