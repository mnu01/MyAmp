#include "display.h"
#include "hub.h"
#include "sequencer.h"
#include <xc.h> 
#include <math.h>

void Display_Init()
{
    TRISA5 = 0;
    TRISC2 = 0;
    TRISB4 = 0;
    
    _Display.IO.ClockPin.Write = &Display_ClockWrite;
    _Display.IO.DataPin.Write = &Display_DataWrite;
    _Display.IO.LatchPin.Write = &Display_LatchWrite;
    
    _Display.SavingMask = 0xFF;
    _Display.iDisplay = 0;
//    _Display.UpdateChannel = true;
    _Display.ReadyToWrite = true;
    _Display.ReadyToRead = false;

    _Display.PrimaryBuffer = (unsigned char*)_Display.Buffer1;
    _Display.SecondaryBuffer = (unsigned char*)_Display.Buffer2;
    
    for (int i = 0; i < DIVISION * (CHANNEL_DIGIT + DIGIPOT); i++)
    {
        *(_Display.PrimaryBuffer + i) = 0x00;
        *(_Display.SecondaryBuffer + i) = 0x00;
    }

    for(int i = 0; i < CHANNEL_DIGIT; i++)
        _Display.DigitalValueRatio[i] = 2 * (int)pow(10, CHANNEL_DIGIT - i - 1);
    
    _Display.Counter = 0;
    _Display.CounterRef = Sequencer_GetCounter(0.015f);
}    

void Display_ClockWrite(bool value)
{
    LATB4 = value;
}

void Display_LatchWrite(bool value)
{
    LATC2 = value;
}

bool Display_DataWrite(bool value)
{
    LATA5 = value;
}

void Display_ProcessData(unsigned char ADigitalValue, unsigned char *AValues, bool ALoaded, bool APushed)
{
    if (_Display.ReadyToWrite)
    {    
        _Display.ReadyToRead = false;
        unsigned char iDataIndex = 0;
        iDataIndex = 0;
        
        for(int i = 0; i < CHANNEL_DIGIT; i++)
            _Display.DigitalValueData[i] = _Digits[(ADigitalValue / _Display.DigitalValueRatio[i]) % 10];
        
        if (ALoaded)
            _Display.DigitalValueData[1] |= DIGIT_POINT & _Display.PointMask[1];
        
        if (APushed)
            _Display.DigitalValueData[0] |= DIGIT_POINT & _Display.PointMask[0];
        
        for(int i = 0; i < DIGIPOT; i++)
            _Display.ValueData[i] = AValues[i] % DIVISION;
        
        for (int k = 0; k < DIVISION; k ++)
        {
            // Display channel
            for (int i = 0; i < CHANNEL_DIGIT; i++)
            {
                *(_Display.SecondaryBuffer + iDataIndex) = _Display.DigitalValueData[i] & _Display.SavingMask; // | (0xFF * (LChannel % 2)) & DIGIT_POINT;
                iDataIndex++;
            }
            
            // Display values
            unsigned char LBit;
            for (int j = 0; j < DIGIPOT; j++)
            {
                unsigned char LData = 0x00;
                for (int i = 0; i < LED; i++)
                {
                    LBit = 0x00;
                    if (AValues[j] >= DIVISION * (i + 1))
                        LBit = 0x01;
                    else
                    {
                        if (AValues[j] > DIVISION * i)
                            LBit = (unsigned char)(_Display.ValueData[j] > k); // _Convert[iDisplay]);
                    }
                    LData |= LBit << i;
                }
                *(_Display.SecondaryBuffer + iDataIndex) = LData;
                iDataIndex++;
            }
        }
        _Display.ReadyToRead = true;
		_Display.ReadyToWrite = false;
    }
}

void Display_SwapBuffer()
{
    unsigned char *LSwap = _Display.PrimaryBuffer;
    _Display.PrimaryBuffer = _Display.SecondaryBuffer;
    _Display.SecondaryBuffer = LSwap;
    
    _Display.ReadyToWrite = true;
    _Display.ReadyToRead = false;
}
