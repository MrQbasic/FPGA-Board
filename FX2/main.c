#include "fx2regs.h"


#define PIN_PROG_B (1 << 2)  //CRTL 2
#define PIN_CCLK (1 << 2)    //PA2
#define PIN_DIN  (1 << 3)    //PA3

#define PIN_DONE (1 << 0)    //PA0 INPUT

#define PIN_FPGA_INPUT (1 << 7) //PB7

void delay(void) {
    unsigned int i;
    unsigned char j;

    // Outer and inner loops tuned for ~1,000 ms execution time
    for (int i = 0; i < 1200; i++) {
        for (int j = 0; j < 1024; j++) {
            __asm
                nop
                nop
            __endasm;
        }
    }
}

void main(void) {
    // 1. Set 8051 CPU clock to 48 MHz
    CPUCS = 0x10;

    //set CCLK and DIN outputs and LOW
    OEA = PIN_CCLK | PIN_DIN;
    IOA &= ~(PIN_CCLK | PIN_DIN); // Start CCLK & DIN Low

    //Set data pin output and LOW
    OEB = PIN_FPGA_INPUT;
    IOB &= ~(BYTE)PIN_FPGA_INPUT;

    // 3. Configure EP2 as Bulk OUT, 512 bytes, Quad buffered
    EP2CFG = 0xA2; 
    EP2BCL = 0x80; // Arm EP2 buffer

    //reset config memory with PROG_B pin
    GPIFCTLCFG = 0x0; //normal outputs not tri-state
    GPIFIDLECTL &= ~PIN_PROG_B;
    for (volatile int i = 0; i < 5000; i++); // ~500ns+ delay
    GPIFIDLECTL |= PIN_PROG_B;

    // Main USB Loop
    while ((IOA & PIN_DONE) == 0) {
        // Check if EP2 has incoming data from USB host (EP2CS empty bit is 0)
        if (!(EP2CS & 0b00000100)) {
            WORD len = ((WORD)EP2BCH << 8) | EP2BCL;

            for (int i = 0; i < len; i++) {
                BYTE byte = EP2FIFOBUF[i];

                // Shift out 8 bits, MSB first
                for (int b = 7; b >= 0; b--) {
                    if (byte & (1 << b)) {
                        IOA |= PIN_DIN;
                    } else {
                        IOA &= ~PIN_DIN;
                    }
                    // Toggle CCLK High then Low
                    IOA |= PIN_CCLK;
                    IOA &= ~PIN_CCLK;
                }
            }

            // Re-arm EP2 to accept the next USB buffer packet
            EP2BCL = 0x80;
        }
    }

    while(1){
        //check for done
        if(IOA & 0x01){
            IOB |= PIN_FPGA_INPUT;
            delay();

            IOB &= ~PIN_FPGA_INPUT;
            delay();
        }
    }
}