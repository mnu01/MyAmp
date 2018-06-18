#include <pic18f2550.h>

#include "hardware.h"

bool Dummy_Read()
{
    return 0x00;
}
    
void Dummy_Write(bool AValue) { }

void InitPin(Pin *APin)
{
//    if (*APin.Read == 0x00)
//        APin->Read = &Dummy_Read;
//    if (APin->Write == 0x00)
//        APin->Write = &Dummy_Write;
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
        AIO.DataPin.Write((AChar >> i) & 0x01);
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
    bool LBit = AIO.DataPin.Read();
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
    AIO.LatchPin.Write(0x00);
}

void LatchOff(Connector AIO)
{
    AIO.LatchPin.Write(0x01);
}

void ClockUp(Connector AIO)
{
    AIO.ClockPin.Write(0x00);
}

void ClockDown(Connector AIO)
{
    AIO.ClockPin.Write(0x01);
}