#include "switch.h"
#include "system.h"
#include "sequencer.h"
#include <xc.h> 

void InitSwitch()
{
    TRISA2 = 0;
    TRISA3 = 0;
    TRISA4 = 0;

    _Switch.Sound[0].Config[0] = 0x2C;
    _Switch.Sound[0].Config[1] = 0x00;
    _Switch.Sound[1].Config[0] = 0xCC;
    _Switch.Sound[1].Config[1] = 0x00;
    _Switch.Sound[2].Config[0] = 0xC0;
    _Switch.Sound[2].Config[1] = 0x00;
    _Switch.Sound[3].Config[0] = 0xD0;
    _Switch.Sound[3].Config[1] = 0x00;
    _Switch.Sound[4].Config[0] = 0xC4;
    _Switch.Sound[4].Config[1] = 0x00;
    _Switch.Counter = 0;
    _Switch.CounterRef = Sequencer_GetCounter(SWITCH_REFRESH);
    _Switch.IO.ClockPin.Write = &Switch_ClockWrite;
    _Switch.IO.DataPin.Write = &Switch_DataWrite;
    _Switch.IO.LatchPin.Write = &Switch_LatchWrite;
    InitConnector(&_Switch.IO);
}

void Switch_ClockWrite(bool value)
{
    LATA4 = value;
}

void Switch_LatchWrite(bool value)
{
    LATA1 = value;
}

bool Switch_DataWrite(bool value)
{
    LATA0 = value;
}

