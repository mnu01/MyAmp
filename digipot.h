/* 
 * File:   digipot.h
 * Author: M'nu
 *
 * Created on 4 avril 2017, 19:26
 */

#include "hardware.h"

#ifndef DIGIPOT_H
#define	DIGIPOT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define DIGIPOT_REFRESH    0.25f // seconds

    typedef struct
    {
        unsigned int CounterRef;
        unsigned int Counter;
        Connector IO;
    } DigiPot;
    

    void DigiPot_ClockWrite(bool value);
    void DigiPot_LatchWrite(bool value);
    bool DigiPot_DataWrite(bool value);

    void InitDigiPot();
    
    DigiPot _DigiPot;

#ifdef	__cplusplus
}
#endif

#endif	/* DIGIPOT_H */

