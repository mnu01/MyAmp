/* 
 * File:   hub.h
 * Author: M'nu
 *
 * Created on 9 mars 2017, 22:15
 */

#ifndef HUB_H
#define	HUB_H

#include "hardware.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define MAX_VALUE       (0x01 << RVAR_LENGHT) - 1
#define ACTION_INDEX    2
#define PUSHED_INDEX    5
#define GAIN_INDEX      0
#define BASS_INDEX      1
#define MIDDLE_INDEX    2
#define TREBLE_INDEX    3
#define VOLUME_INDEX    4
    
    typedef struct 
    {
        bool ActionButton;
        bool PushedButton;
        unsigned char Channel;
        unsigned char Sound;
        unsigned char Values[DIGIPOT];
    } DataBuffer;

    typedef struct
    {
        bool Switch[HUB_REG * 8];
        bool SwitchOld[HUB_REG * 8];
        bool ReadComplete;
        unsigned char iRead;
        DataBuffer PrimaryBuffer;
        DataBuffer SecondaryBuffer;
        DataBuffer BackupBuffer;
        bool ReadyToWrite;
        bool ReadyToRead;
        bool Paused;
        unsigned short MaxChannel;
        unsigned char MaxSound;
        unsigned char CurrentChannel;
        unsigned char CurrentSound;
        Connector IO;
    } Hub;
    
    void Hub_ClockWrite(bool value);
    void Hub_LatchWrite(bool value);
    bool Hub_DataRead();

    void Hub_Init();
    void Hub_ReadSwitch();
    signed char Hub_DecodeSwitch(unsigned char AIndex1, unsigned char AIndex2);
    void Hub_SetValue(unsigned char *ASourceValue, signed char AMinValue, signed int AMaxValue, signed char AIncrement);
    void Hub_UpdateValues();
    void Hub_CopyBuffer();
    
    Hub _Hub;

#ifdef	__cplusplus
}
#endif

#endif	/* HUB_H */

