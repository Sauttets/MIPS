#include <msp430.h>
#include "../base.h"
#include "TA1.h"
#include "event.h"

// For Hysteresis
#define CNT_MAX 5
const UChar bit_mask[2] = {BIT1, BIT0};
const TEvent event[2] = {EVENT_BTN1, EVENT_BTN2};
static UChar cnt[2];
static UChar state[2];

#pragma FUNC_ALWAYS_INLINE(TA1_init)
GLOBAL Void TA1_init(Void)
{
	TA1CTL   = 0; 	// stop mode, disable and clear flags
	TA1CCTL0 = 0; 	// no capture mode, compare mode
    				// clear and disable interrupt flag
    TA1CCR0 = 48-1; 		// set up Compare Register
    TA1EX0 = TAIDEX_7; 		// set up expansion register
    TA1CTL = TASSEL__ACLK 	// 614.4 kHz
    		| MC__UP 		//Up Mode
			| ID__8 		// /8
			| TACLR 		// clear and start Timer
			| TAIE; 		// enable interrupt
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt Void TIMER1_A1_ISR(Void)
{
	static UChar index;
    Bool btn = TSTBIT(P1IN, bit_mask[index]); // button is selected and checked if it is pressed

    /* Hysteresis Button */
    if(btn) { // is the button pressed?
        if(cnt[index] < CNT_MAX) // is the counter less than Max (prevents cnt from rising above max)
        {
            cnt[index]++;
            if(cnt[index] == CNT_MAX) // is the new counter (+1) equal to cnt_max?
            {
                if(state[index] == 0) // is Event already set?
                {
                    state[index] = 1;
                    Event_set(event[index]); // Set Event
                    __low_power_mode_off_on_exit(); // Exit Low Power Mode after Exit
                }
            }
        }
    } else { // is the button not pressed?
        if(cnt[index] > 0) // as long as the counter is greater than 0
        {
            cnt[index]--; // decrease it by 1
        }
        if(cnt[index] == 0)
        {
            state[index] = 0;
        }
    }

    // Switch Button for next Interrupt
    if(index == 1)
    {
        index--;
    } else {
        index++;
    }

    CLRBIT(TA1CTL, TAIFG);
}
