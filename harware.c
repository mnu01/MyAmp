#include <pic18f2550.h>

#include "hardware.h"

void InitPin(Pin *APin)
{
    APin->NegativeMask = 0x01 << APin->PinNumber;
    APin->Mask = ~APin->NegativeMask;
}

void InitConnector(Connector *AConnector)
{
    InitPin(&AConnector->ClockPin);
    InitPin(&AConnector->DataPin);
    InitPin(&AConnector->LatchPin);
}

void WriteChar(unsigned char AChar, Connector AIO)
{
    for (int i = 7; i >= 0; i--)
    {
        ClockUp(AIO);
        *AIO.DataPin.Port = *AIO.DataPin.Port & AIO.DataPin.Mask | ((AChar >> i) & 0x01) << AIO.DataPin.PinNumber;
        ClockDown(AIO);
    }
}

void WriteData(unsigned char *AData, unsigned char ASize, Connector AIO)
{
    LatchOn(AIO);
    for (int i = ASize - 1; i >= 0; i--)
        WriteChar(AData[i], AIO);
    LatchOff(AIO);
}

bool ReadBit(Connector AIO)
{
    bool LBit = (*AIO.DataPin.Port >> AIO.DataPin.PinNumber) & 0x01;
    ClockUp(AIO);
    ClockDown(AIO);
    return LBit;
}

unsigned char ReadChar(Connector AIO)
{
    unsigned char LChar = 0x00;
    for (int i = 7; i >= 0; i--)
        LChar |= ReadBit(AIO) >> i;
    return LChar;
}

unsigned char *ReadData(unsigned char ASize, Connector AIO)
{
    unsigned char *LData;
    LatchOn(AIO);
    for (int i = ASize - 1; i >= 0; i--)
        LData[i] = ReadChar(AIO);
    LatchOff(AIO);
    return LData;
}

void ReadBitArray(bool *AData, Connector AIO)
{
//    ClockUp(AIO);
    // Load is the same procedure as latch, but inverted
    for (int i = 0; i < BATCH_READ; i++)
        AData[i] = ReadBit(AIO);
}

void LatchOn(Connector AIO)
{
    *AIO.LatchPin.Port &= AIO.LatchPin.Mask;
}

void LatchOff(Connector AIO)
{
    *AIO.LatchPin.Port |= AIO.LatchPin.NegativeMask;
}

void ClockUp(Connector AIO)
{
    *AIO.ClockPin.Port &= AIO.ClockPin.Mask;
}

void ClockDown(Connector AIO)
{
    *AIO.ClockPin.Port |= AIO.ClockPin.NegativeMask;
}