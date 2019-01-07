/* 
 * File:   switch.h
 * Author: M'nu
 *
 * Created on 21 juin 2018, 18:38
 */

#include "hardware.h"

#ifndef SWITCH_H
#define	SWITCH_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SWITCH_REFRESH    0.5f // seconds
    
    typedef struct
    {
        unsigned char Config[SWITCH_REG];        
    } SoundConfig;

    typedef struct
    {
        SoundConfig Sound[5];
        unsigned int CounterRef;
        unsigned int Counter;
        Connector IO;
    } Switch;

    void Switch_ClockWrite(bool value);
    void Switch_LatchWrite(bool value);
    bool Switch_DataWrite(bool value);

    void InitSwitch();
    
    Switch _Switch;

#ifdef	__cplusplus
}
#endif

#endif	/* SWITCH_H */

