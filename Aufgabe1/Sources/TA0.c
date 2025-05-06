#include <msp430.h>
#include "../base.h"
#include "TA0.h"
#include "event.h"
#include <stdio.h>

#define HIGH 0x80
#define LOW 0x00

#define ACKFRQ 614.4 // kHz
#define TIMEBASE 50  //ms
#define PATTERN_COUNT 6
#define SCALING ((UInt)(ACKFRQ * TIMEBASE))
#define TICK(t) ((SCALING / 64) * ((t) / TIMEBASE) - 1)


LOCAL const UInt blink_pattern[] =
{
    HIGH | TICK(2000), LOW | TICK(500), 0,
    HIGH | TICK(750), LOW | TICK(750), 0,
    HIGH | TICK(250), LOW | TICK(250), 0,
    LOW | TICK(500), HIGH | TICK(500), LOW | TICK(1500), 0,
    LOW | TICK(500), HIGH | TICK(500), LOW | TICK(500), HIGH | TICK(500), LOW | TICK(1500), 0,
    LOW | TICK(500), HIGH | TICK(500), LOW | TICK(500), HIGH | TICK(500), LOW | TICK(500), HIGH | TICK(500), LOW | TICK(1500), 0
};

const UInt *const pattern_table[PATTERN_COUNT] =
{
    &blink_pattern[0], &blink_pattern[3], &blink_pattern[6], &blink_pattern[9], &blink_pattern[13], &blink_pattern[19]
};

static UChar pattern_index_new_selected; 
GLOBAL Void set_blink_muster(UInt arg)
{
    pattern_index_new_selected = arg;
}

#pragma FUNC_ALWAYS_INLINE(TA0_init)
GLOBAL Void TA0_init(Void)
{
    pattern_index_new_selected = MUSTER1;

   TA0CTL   = 0; // stop mode, disable and clear flags
   TA0CCTL0 = 0; // no capture mode, compare mode
                 // clear and disable interrupt flag
   TA0CCR0  = 0xFFFF;       // set up Compare Register
   TA0EX0   = TAIDEX_0;     // set up expansion register
   TA0CTL   = TASSEL__ACLK  // 613.75 kHz
            | MC__UP        // Up Mode
            | ID__1         // input divider
            | TACLR         // clear and start Timer
            | TAIE          // enable interrupt
            | TAIFG;        // set interrupt flag
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt Void TIMER0_A1_ISR(Void)
{
    static UChar pattern_index;
    static UChar array_index_phase;

    if((*(pattern_table[pattern_index] + array_index_phase)) EQ 0) { // If end of phase, next pattern
        array_index_phase = 0;
        pattern_index = pattern_index_new_selected;
    }
    if(TSTBIT(*(pattern_table[pattern_index] + array_index_phase), HIGH)) { // Depending on phase bit, turn LED on/off
        SETBIT(P1OUT, BIT2);
    } else {
        CLRBIT(P1OUT, BIT2);
    }
    TA0CCR0 = ~HIGH BAND (*(pattern_table[pattern_index] + array_index_phase));
    array_index_phase++;

    CLRBIT(TA0CTL, TAIFG);
}
