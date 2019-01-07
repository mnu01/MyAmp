/* 
 * File:   hardware.h
 * Author: M'nu
 *
 * Created on 9 mars 2017, 18:51
 */

#ifndef HARDWARE_H
#define	HARDWARE_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define LED             7
#define DIGIPOT         8
#define CHANNEL_DIGIT   2
#define RVAR            8
#define RVAR_LENGHT     6
#define HUB_REG         6
#define BATCH_READ      24
#define INPUT_COUNT     HUB_REG * 8
#define SWITCH_REG      2
    typedef bool (*FREAD)(void);
    typedef void (*FWRITE)(bool);
   
    typedef struct 
    {
        FREAD Read;
        FWRITE Write;
//        volatile unsigned char *Port;
//        unsigned char PinNumber;
//        unsigned char Mask;
//        unsigned char NegativeMask;
    }Pin;

    typedef struct 
    {
        Pin ClockPin;
        Pin DataPin;
        Pin LatchPin;
    }Connector;
    
    typedef struct
    {
        Pin ClockPin;
        Pin DataInPin;
        Pin DataOutPin;
    }BiDirectionalConnector;    
        
    void WriteData(unsigned char *AData, unsigned char ASize, Connector AIO);
    void WriteChar(unsigned char AData, Connector AIO);
    unsigned char ReadChar(Connector AIO);
    unsigned char *ReadData(unsigned char ASize, Connector AIO);
    bool ReadBit(Connector AIO);
    void ReadBitArray(bool *AData, Connector AIO);
    void LatchOn(Connector AIO);
    void LatchOff(Connector AIO);
    void ClockUp(Connector AIO);
    void ClockDown(Connector AIO);

#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

