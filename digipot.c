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
    _DigiPot.IO.ClockPin.Port = &LATA;
    _DigiPot.IO.ClockPin.PinNumber = 4;
    _DigiPot.IO.DataPin.Port = &LATA;
    _DigiPot.IO.DataPin.PinNumber = 2;
    _DigiPot.IO.LatchPin.Port = &LATA;
    _DigiPot.IO.LatchPin.PinNumber = 3;
    InitConnector(&_DigiPot.IO);
}
