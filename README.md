# About

This program is designed to work with specific hardware, described
below.

BOM:

* PIC18F46K80 uController
* MCP4922 12-bit, dual-channel DAC
* CD4051 1x8 multiplexer
* TL084 op-amp (2)
* 100N capacitors
* 6 Pin programmer header
* 10K resistor
* 470R resistor

Description:

In this circuit, the PIC is the SPI master and the MCP4922 DAC is the
slave. The SPI bus is configured in (0,0) mode with FOSC/4 clock, which
is 4Mhz, since the master clock is 16Mhz.

PORTD.D0 is used as the chip select line for the MCP4922. PORTD.D1 is
used as the enable bit for the 4051 multiplexer.  Both ICs are disabled
when the lines are high (as per the norm) and enabled with the lines
are low.

PORTB (B0, B1, B2) are used as select lines for the multiplexer.

The op-amps are set up as sample/hold buffers. Since the multiplexer
has eight outputs, the circuit produces 8 control voltages and uses
8 buffers.

Each buffer has the output tied to the negative input. There is a 100N
capacitor tying the positive input to ground, which holds the charge
set from the DAC.

Every IC has its supply lines correctly bypassed with 100N caps.

The resistors are used in the canonical RESET circuit for the PIC, which
essentially just ties MCLR to high. 

The output of DAC channel A is routed to the multiplexer input. Next, 
each of the multiplexer outputs is wired to an S&H buffer.

The main program loops through, doing the following:

1. Inhibits routing by the 4051 by bringing it's ENABLE line HIGH
2. Selects and output on the multiplexer
3. Updates the value that will be sent to the DAC.
4. Writes data to the DAC
5. Enable the 4051 by bringing the line low.
6. Wait for a period of time in order to allow the S&H capacitor to charge
7. Inhibit routing by the 4051 by bringing its ENABLE line HIGH

Note that several of the steps above could be streamlined. See code
comments for details.
