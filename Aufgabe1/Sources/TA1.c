#include <msp430.h>
#include "../base.h"
#include "TA1.h"
#include "event.h"

// For Hysteresis
#define CNT_MAX 5 // Anzahl gleicherbleibender werte
#define BTN_CNT 2
const UChar bit_mask[BTN_CNT] = {BIT1, BIT0};
const TEvent event[BTN_CNT] = {EVENT_BTN1, EVENT_BTN2};

static UChar cnt[BTN_CNT];    // Zähler für jedes Sample-Fenster
static UChar state[BTN_CNT];  // stabiler Zustand: 0=frei, 1=gedrückt

#pragma FUNC_ALWAYS_INLINE(TA1_init)
GLOBAL Void TA1_init(Void)
{
	TA1CTL   = 0; 	// stop mode, disable and clear flags
	TA1CCTL0 = 0; 	// no capture mode, compare mode
    				// clear and disable interrupt flag
    TA1CCR0 = 24-1; 		// set up Compare Register (2,5 ms)
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
    Bool btn = TSTBIT(P1IN, bit_mask[index]); // 1 = Taster nicht gedrückt; 0 = gedrückt

    if(btn) {
        if(cnt[index] < CNT_MAX) // is the counter less than Max (prevents cnt from rising above max)
        {
            cnt[index]++;
            if(cnt[index] == CNT_MAX) // neuer counter = max?
            {
                if(state[index] == 0) // bereits gesetztes event
                {
                    state[index] = 1;
                    Event_set(event[index]); // main bearbeitet das event
                    __low_power_mode_off_on_exit(); //
                }
            }
        }
    } else {  // button wechseln
        if(cnt[index] > 0)
        {
            cnt[index]--;
        }
        if(cnt[index] == 0)
        {
            state[index] = 0;
        }
    }

    //Button tauschen für nächsten durchgang
    if(index == 1)
    {
        index--;
    } else {
        index++;
    }

    CLRBIT(TA1CTL, TAIFG);
}
