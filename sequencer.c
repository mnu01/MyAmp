#include "sequencer.h"
#include "system.h"

#include <xc.h>
#include <math.h>

void Sequencer_Start()
{
    // Prescaler div by 256 (111) = 76Hz / 9; 32(100) = 690Hz / 9
    T0PS0 = 0;
    T0PS1 = 1;
    T0PS2 = 1;

    PSA = 0;      //Timer Clock Source is from Prescaler
    T0CS = 0;     //Prescaler gets clock from FCPU (5MHz)
    T08BIT = 1;   //8 BIT MODE

    TMR0IE = 1;   //Enable TIMER0 Interrupt
    TMR0ON = 1;   //Now start the timer!
    
    unsigned char LPrescaler = T0PS0 | (T0PS1 << 1) | (T0PS2 << 2);
    long LTickCount = pow(2, LPrescaler) * 4096;
    _Timer.Period = (1.0f * LTickCount / _XTAL_FREQ);
}

unsigned char Sequencer_GetCounter(float ATime)
{
    return ATime / _Timer.Period;
}

void Sequencer_StopTimer()
{
    TMR1ON = 0;
    _Timer.Counter = 0;
}

void Sequencer_StartTimer(float ATime, bool ALoop)
{
    _Timer.Loop = ALoop;
    _Timer.Ticks = ATime / ((4.0f / SYS_FREQ) * 1 * 65535);
    
    T1CKPS0 = 0;   // bits 5-4  Prescaler Rate Select bits
    T1CKPS0 = 0;   // bit 4
    T1OSCEN = 1;   // bit 3 Timer1 Oscillator Enable Control bit 1 = on
    T1SYNC = 1;    // bit 2 Timer1 External Clock Input Synchronization Control bit...1 = Do not synchronize external clock input
    TMR1CS = 0;    // bit 1 Timer1 Clock Source Select bit...0 = Internal clock (FOSC/4)
    TMR1H = 0;    // preset for timer1 MSB register
    TMR1L = 0;    // preset for timer1 LSB register
 
    // Interrupt Registers
    TMR1IE = 1;    // enable Timer1 interrupts
    TMR1IF = 0;    // bit2 clear timer 0 interrupt flag
    TMR1ON = 1;    // bit 0 enables timer
}

