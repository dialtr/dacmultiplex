/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.78.1
        Device            :  PIC18F46K80
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include <pic18f46k80.h>

#include "mcc_generated_files/mcc.h"

/*
                         Main application
 */

void dac_init() {
    LATDbits.LATD0 = 1; // Deselect DAC (HIGH = not enabled))
    LATDbits.LATD1 = 1; // Deselect 4051 (HIGH = not enabled))
    LATB = 0;
}

void dac_write_a(unsigned short val) {
    // Select chip
    LATDbits.LATD0 = 0;
    NOP();
    NOP();
    
    char b1 = 0b00110000;
    b1 |= ((val >> 8) & 0x0f);
    
    char b2 = (val & 0xff);
    
    SPI_ExchangeByte(b1);
    SPI_ExchangeByte(b2);
    
    // Deselect chip
    LATDbits.LATD0 = 1;
}

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    bool opened = SPI_Open(SPI_DEFAULT);
    
    unsigned short val = 0;
    while (1) {
        for (char i = 0; i < 8; ++i) {
            // Inhibit routing signal on 4051
            LATDbits.LATD1 = 1;
            // Wait for line to settle
            // TODO(tdial): How long do I have to wait?
            // These NOPs are probably not needed since the dac_write
            // funtion will cost some cycles and the lines will have
            // settled by the time that we enable the 4051.
            NOP(); NOP(); NOP(); NOP(); // 1 uS
              
            // Select multiplexer address on 4051 (0..7)
            LATB = i;
                        
            // Wait for lines to settle
            // TODO(tdial): How long do I have to wait?
            // We probably also do not need these NOPs.
            NOP(); NOP(); NOP(); NOP(); // 1 uS                       
            
            ++val;
            if (val == 4096) {
                val = 0;
            }
            
            int value = val + ((int)i) * 512;
            value = value % 4096;
            
            dac_write_a(value);
            
            /*
            // Write voltage
            if (i == 0) {
                dac_write_a(4095);
            } else {
                dac_write_a(0);
            }
            */
            
            // Bring enable line on 4051 low to enable routing of CV
            // to the correct 
            LATDbits.LATD1 = 0;
            
            // Wait for charge (These delays approx 4 microseconds at 16 mhz cpu clock)
            // TODO(tdial): do the R/C charge calculation again. I believe that
            // this is quite conservative, though, given the 100N cap that's
            // being used in the sample/hold buffer, and the fact there's no
            // resistor in place.
            for (int nop = 0; nop < 40; ++nop) {
                NOP();
            }
           
            // Charge complete; inhibit 
            LATDbits.LATD1 = 1;                        
        }       
    }
    
    SPI_Close();
}
/**
 End of File
*/