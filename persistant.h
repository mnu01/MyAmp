/* 
 * File:   persistant.h
 * Author: M'nu
 *
 * Created on 9 mars 2017, 22:52
 */

#ifndef PERSISTANT_H
#define	PERSISTANT_H

#include "hardware.h"
#include "hub.h"
#include "zip.h"
#include "digipot.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
   
    typedef struct
    {
        unsigned char Channel;
        unsigned char MidiChannel;
        unsigned char ControlChange[DIGIPOT];
        unsigned char Switch[2];
    } Config;

//    typedef enum _Action { Read, Write } Action;
    
    bool Persistant_ControlByte(int AAddress, bool ARead);
    bool Persistant_Address(int AAddress, bool ARead);
    void Persistant_Init();
    unsigned char Persistant_ReadExternalMemory(int AAddress);
    bool Persistant_WriteExternalMemory(int AAddress, unsigned char AData);
    unsigned char Persistant_ReadInternalMemory(int AAddress);
    bool Persistant_WriteInternalMemory(int AAddress, unsigned char AData);
    bool Persistant_SaveBuffer(DataBuffer ABuffer);
    bool Persistant_LoadBuffer(DataBuffer *ABuffer);
    
    Config _Config;

#ifdef	__cplusplus
}
#endif

#endif	/* PERSISTANT_H */

