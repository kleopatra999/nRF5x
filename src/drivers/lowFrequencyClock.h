
/*
 * Low frequence oscillator (not really a clock.)
 *
 * The states are: !started, started, !running, running
 * started does not guarantee running!!!
 *
 * Valid call sequences:
 *
 * The normal call sequence:
 * configureXtalSource, start, isStarted returns true, while(!isRunning()) {}, isRunning returns true
 *
 * start, isStarted, ... (uses default LFRC oscillator source)
 *
 * You must busy wait for isRunning:
 * isStarted returns false, start, isStarted returns true, isRunning returns false
 *
 * You must configure before starting:
 * start, configureXtalSource will halt
 *
 *
 */
class LowFrequencyClock {
public:
	static void configureXtalSource();

	static void start();


	/*
	 * Was there a prior call to start() AND did the LFCKLSTARTED event occur?
	 * !!! Does not guarantee isRunning()
	 */
	static bool isStarted();


	/*
	 * Is clock running (and stable)?
	 */
	static bool isRunning();

	/*
	 * !!! No stop() defined.
	 * (Not needed by most apps.)
	 * Assume the PowerManager functions as designed and does not power down clock.
	 */
};
