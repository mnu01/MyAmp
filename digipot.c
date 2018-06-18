#include "digipot.h"
#include "system.h"
#include <xc.h> 
#include <math.h>
#include <stdint.h>

void InitDigiPot()
{
    unsigned char LPrescaler = T0PS0 | (T0PS1 << 1) | (T0PS2 << 2);
    long LTickCount = pow(2, LPrescaler) * 4096;
    float LPeriod = (1.0f * LTickCount / _XTAL_FREQ);
    
    TRISA2 = 0;
    TRISA3 = 0;
    TRISA4 = 0;

    _DigiPot.CounterRef = DIGIPOT_REFRESH / LPeriod;
    _DigiPot.Counter = 0;
    _DigiPot.IO.ClockPin.Write = &DigiPot_ClockWrite;
    _DigiPot.IO.DataPin.Write = &DigiPot_DataWrite;
    _DigiPot.IO.LatchPin.Write = &DigiPot_LatchWrite;
    InitConnector(&_DigiPot.IO);
}

void DigiPot_ClockWrite(bool value)
{
    LATA4 = value;
}

void DigiPot_LatchWrite(bool value)
{
    LATA3 = value;
}

bool DigiPot_DataWrite(bool value)
{
    LATA2 = value;
}
