/* 
 * File:   display.h
 * Author: M'nu
 *
 * Created on 9 mars 2017, 18:56
 */

#include "hardware.h"
#include "hub.h"
#include <stdbool.h>

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif
    
//    _1
//64|128|2
//32| _ |4
//    16*8

#define DIGIT_ONE       0x06
#define DIGIT_TWO       0xB3
#define DIGIT_THREE     0x97
#define DIGIT_FOR       0xC6
#define DIGIT_FIVE      0xD5
#define DIGIT_SIX       0xF5
#define DIGIT_SEVEN     0x07
#define DIGIT_HEIGHT    0xF7
#define DIGIT_NINE      0xD7
#define DIGIT_ZERO      0x77
#define DIGIT_POINT     0x08
#define DIGIT_A         0xE7
#define DIGIT_B         0xF4
#define DIGIT_C         0x71
#define DIGIT_D         0xB6
#define DIGIT_E         0xF1
#define DIGIT_F         0xE1
#define DIGIT_G         0x75
#define DIGIT_H         0xE6
    
#define DIVISION        (char)((MAX_VALUE * 1.0f) / (LED * 1.0f))

    unsigned char _Digits[] = 
    { 
        DIGIT_ZERO, 
        DIGIT_ONE, 
        DIGIT_TWO, 
        DIGIT_THREE, 
        DIGIT_FOR, 
        DIGIT_FIVE, 
        DIGIT_SIX, 
        DIGIT_SEVEN, 
        DIGIT_HEIGHT ,
        DIGIT_NINE
//        DIGIT_A,
//        DIGIT_B,
//        DIGIT_C,
//        DIGIT_D,
//        DIGIT_E,
//        DIGIT_F 
    };
    typedef struct
    {
        bool ReadyToRead;
        bool ReadyToWrite;
        unsigned char Buffer1[DIVISION][DIGIPOT + CHANNEL_DIGIT];
        unsigned char Buffer2[DIVISION][DIGIPOT + CHANNEL_DIGIT];
        unsigned char *PrimaryBuffer;
        unsigned char *SecondaryBuffer;
        unsigned char DigitalValueRatio[CHANNEL_DIGIT];
        unsigned char DigitalValueData[CHANNEL_DIGIT];
        unsigned char ValueData[DIGIPOT];
        unsigned char iDisplay;
//        bool UpdateChannel;
//        bool UpdateValues[DIGIPOT];
        unsigned char SavingMask;
        unsigned char PointMask[CHANNEL_DIGIT];
        Connector IO;
    } Display;

    void Display_Init();
    void Display_ProcessData(unsigned char AChannel, unsigned char *AValues, bool ALoaded, bool APushed);
    void Display_SwapBuffer();
    
    Display _Display;

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

