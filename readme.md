
Sparse, object oriented (C++) library for nRF5x family embedded radios.


Provides
-
      
     Wrappers around certain RTC peripherals of the Nordic nRF5x family of embedded radio SoC chips.
     Other simple services such as a mailbox
     Wrappers around certain board-level devices such as LED's.
     Exception handlers.


Motivation
-
    Nordic's libraries suspect and overweight for my use case.
    I wanted a lightweight radio protocol
    I didn't think I needed an RTOS, but needed some basics e.g. timers and mailbox

I had the most trouble with Nordic's app_timer library.
    
Implementation Notes
-
Some use of HAL but also some bare metal, direct access to device registers.  (Probably HAL should be used exclusively so it could be ported to another chip?)

Not using Nordics C-language drivers in NRF\_SDK/components/nrf_drv.  Those drivers may be more robust, but are hard to read and understand.


Building
-

Several build configurations:

    artifacts are libraries cross-compiled for targets nRF51 and nRF52 (ARM M0 and M4.)
    artifacts are executables that test the library

The project in this repository is an Eclipse project, using Eclipse's build system.
The project's Eclipse build configuration contains many include paths and other build settings.
Such settings are not defined in a Makefile.

The most important setting is *Properties>C/C++ Build>Build Variables* variable named NRF_SDK should have a value pointing to your installation of the Nordic SDK.

Other important settings:

    linker setting --specs=nosys.specs otherwise get undefined reference to _exit.
    Tool Settings>Target Processor  float abi hard (nRF52 has FPU)
    *Properties>C/C++ General>Paths and Symbols>Symbols* define NRF52 or NRF51, and a board e.g. PC10040?
    
Other settings:

    *Properties>C/C++ General>Paths and Symbols>Symbols* define LOGGING to make Logger potent

AFAIK, many of the other defines for PAN's (Product Anomalies) found in Nordic Makefiles are not needed for this library.  They seem to affect code conditionally compiled into certain Nordic libraries.


Debugging
-

Eclipse with the GNU ARM Plugin supports the nRF52DK and its Segger debug probe nicely.  Plug in your DK dev kit's USB and choose *Debug As>Debug Configurations>nRF5x DebugNRF52*.

(You may want to create your own Debug Configuration.)

To see the log, open a terminal and run JLinkRTTClient, which you can download from Segger.

Testing nRFCounter, you should see a sequence of times scroll by.

TODO add other mains that test other modules.

Boards
-
Some services (e.g. ledLogger) depend on the board configuration (what pins are configured as digital out to LEDs, and what revision of the chip is on the board.)

One build configuration for the nRF51 family defines BOARDS\_CUSTOM which means included Nordic file boards.h will include custom_board.h for RedBear Nano (one LED) from this directory.  

Another build configuration for nRF52 family builds for the nRF52DK board (four LED's.)  Defines BOARD_PCA10040.

Exception Handlers
-
Also called faults (not to be confused with C++ exceptions.)

Two kinds of faults handled:

    hw faults e.g. HardFault_Handler
    sw faults e.g. the handler called by the assert() macro

The library overrides certain default handlers.  Default handlers enter an infinite loop, without entering low power mode.  The override default handlers:

    for production, reset instead of infinite loop
    for debug using a low-reserve power supply, reduce power before sleeping
    
For production, soft reset is preferable to infinite loop so the system might recover, without human intervention to hard reset.
 
For debug, reducing power and sleeping keeps the system in a debuggable state (otherwise an infinite loop quickly causes brown out.)
 
(Future: for debug, the handlers should save system registers from the program context in variables i.e. save the top level of the stack trace.)


See Also
-
Derived from (and superseding) nRFCounter and nRFrawProtocol
 
See testnRFCounter project, a template project with similar build and debug configurations.  More explanation there about exactly how to configure a Build Configuration in Eclipse.

A using project will be fireFly.

Environment
-
I use:

     nRF52DK development board having SEGGER debug probe
     RedBear Nano having nRF51 (debugged via SWC cables to nRF52DK)
     Ubuntu 16.04 LTS
     Eclipse Neon version
     Eclipse ARM GCC plugin
     Nordic NRF_SDK v12

