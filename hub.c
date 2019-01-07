#include "hub.h"
#include "display.h"
#include "zip.h"
#include "buffer.h"
#include "sequencer.h"
#include <xc.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

void Hub_Init()
{
//    _Hub.ReadyToWrite = false;
//    _Hub.ReadyToRead = true;
//    _Hub.Paused = false;
    _Hub.iRead = 0;
    _Hub.Counter = 0;
    _Hub.CounterRef = Sequencer_GetCounter(HUB_REFRESH);

    _Hub.PrimaryBuffer.ActionButton = false;
    _Hub.PrimaryBuffer.PushedButton = false;
    _Hub.PrimaryBuffer.Channel = 0x00;
    _Hub.PrimaryBuffer.Sound = 0x00;
    _Hub.CurrentChannel = _Hub.PrimaryBuffer.Channel;
    _Hub.CurrentSound = _Hub.PrimaryBuffer.Sound;
//    _Hub.MaxChannel = (pow(10, CHANNEL_DIGIT) - 1) * 2;
    _Hub.MaxChannel = (char)((1024.0f / ZIP_LENGTH) * 2.0f);
    _Hub.MaxSound = 4.0f * 2.0f;

    TRISB2 = 1;
    TRISB3 = 0;
    TRISB4 = 0;

    _Hub.IO.ClockPin.Write = &Hub_ClockWrite;
    _Hub.IO.DataPin.Read = &Hub_DataRead;
    _Hub.IO.LatchPin.Write = &Hub_LatchWrite;

    // init values (maybe useless)
    for (int i = 0; i < sizeof(_Hub.PrimaryBuffer.Values); i++)
        _Hub.PrimaryBuffer.Values[i] = 0;
    Hub_ReadSwitch();
}

void Hub_ClockWrite(bool value)
{
    LATB4 = value;
}

void Hub_LatchWrite(bool value)
{
    LATB3 = value;
}

bool Hub_DataRead()
{
    return PORTBbits.RB2;
}

void Hub_ReadSwitch()
{    
    if (_Hub.iRead == 0)
    {
        LatchOn(_Hub.IO);
        LatchOff(_Hub.IO);
    }

    if ((_Hub.iRead < INPUT_COUNT) && (_HubBuffer.Size < BUFFER_SIZE))
    {
        bool LBitArray[BATCH_READ];
        ReadBitArray(LBitArray, _Hub.IO);
        Buffer_Write(LBitArray, &_HubBuffer);
        _Hub.iRead += BATCH_READ; 
    }
    
    if (_Hub.iRead >= INPUT_COUNT)
        _Hub.iRead = 0;
}

signed char Hub_DecodeSwitch(unsigned char AIndex1, unsigned char AIndex2)
{
    // SW1: 00110011
    // SW2: 01100110
    if (_Hub.Switch[AIndex1] == _Hub.SwitchOld[AIndex1] && _Hub.Switch[AIndex1] == _Hub.SwitchOld[AIndex2] && _Hub.SwitchOld[AIndex2] == !_Hub.Switch[AIndex2] || 
        _Hub.Switch[AIndex2] == _Hub.SwitchOld[AIndex2] && _Hub.Switch[AIndex2] == _Hub.Switch[AIndex1] && _Hub.Switch[AIndex2] == !_Hub.SwitchOld[AIndex1])
    {
        return 1;
    }

    // SW1: 01100110
    // SW2: 00110011
    if (_Hub.Switch[AIndex2] == _Hub.SwitchOld[AIndex2] && _Hub.Switch[AIndex2] == _Hub.SwitchOld[AIndex1] && _Hub.SwitchOld[AIndex1] == !_Hub.Switch[AIndex1] || 
        _Hub.Switch[AIndex1] == _Hub.SwitchOld[AIndex1] && _Hub.Switch[AIndex1] == _Hub.Switch[AIndex2] && _Hub.Switch[AIndex1] == !_Hub.SwitchOld[AIndex2])
    {
        return -1;
    }
    
    return 0;
}

void Hub_SetValue(unsigned char *ASourceValue, signed char AMinValue, signed int AMaxValue, signed char AIncrement)
{
    signed int LValue = *ASourceValue;
    LValue += AIncrement;
    if (LValue < AMinValue) LValue = AMinValue;
    if (LValue > AMaxValue) LValue = AMaxValue;
    *ASourceValue = LValue;
}

void Hub_UpdateValues()
{
    if ((_HubBuffer.Size * BATCH_READ) < INPUT_COUNT)
        return;
    
    memcpy(_Hub.SwitchOld, _Hub.Switch, sizeof(_Hub.Switch));
    
    bool LBitArray[BATCH_READ];
    for(char i = 0; i < INPUT_COUNT; i += BATCH_READ)
    {
        if (Buffer_Read(LBitArray, &_HubBuffer))
            memcpy(_Hub.Switch + i, LBitArray, BATCH_READ);
    }

    _Hub.PrimaryBuffer.ActionButton = _Hub.Switch[ACTION_INDEX];
    if (_Hub.Switch[PUSHED_INDEX] && !_Hub.SwitchOld[PUSHED_INDEX])
        _Hub.PrimaryBuffer.PushedButton = !_Hub.PrimaryBuffer.PushedButton;

    if (_Hub.PrimaryBuffer.PushedButton)
        Hub_SetValue(&_Hub.PrimaryBuffer.Sound, 0, _Hub.MaxSound, Hub_DecodeSwitch(0, 1));
    else
        Hub_SetValue(&_Hub.PrimaryBuffer.Channel, 0, _Hub.MaxChannel, Hub_DecodeSwitch(0, 1));

    for (int i = 0; i < sizeof(_Hub.PrimaryBuffer.Values); i++)
    {
       Hub_SetValue(&_Hub.PrimaryBuffer.Values[i], 0, MAX_VALUE, Hub_DecodeSwitch(i * 3 + 3, i * 3 + 4));
    }
}
//
//void Hub_CopyBuffer()
//{
//    memcpy(&_Hub.SecondaryBuffer, &_Hub.PrimaryBuffer, sizeof(DataBuffer));
//    
//    _Hub.ReadyToWrite = false;
//}

