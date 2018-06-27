#include "digipot.h"
#include "system.h"
#include "sequencer.h"
#include <xc.h> 
#include <stdint.h>

void InitDigiPot()
{    
    TRISA2 = 0;
    TRISA3 = 0;
    TRISA4 = 0;

    _DigiPot.Counter = 0;
    _DigiPot.CounterRef = Sequencer_GetCounter(DIGIPOT_REFRESH);
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
