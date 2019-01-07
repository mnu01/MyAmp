/* 
 * File:   midi.h
 * Author: M'nu
 *
 * Created on 12 avril 2017, 21:22
 */

#ifndef MIDI_H
#define	MIDI_H

#include <stdbool.h>
#include "buffer.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
    typedef struct
    {
        unsigned char Code;
        unsigned char Channel;
        unsigned char Data[2];
        signed char DataCount;
    } MidiData;
    
    typedef struct
    {
        MidiData Buffer[BUFFER_SIZE];
        unsigned char iBufferRead;
        unsigned char iBufferWrite;
        unsigned char iData;
        unsigned char Size;
    } Midi;

    void Midi_Init();
    void Midi_ResetBuffer(Midi *AMidiBuffer);
    void Midi_Write(unsigned char AData, Midi *AMidiBuffer);
    bool Midi_Read(MidiData *AData, Midi *AMidiBuffer);
    
    Midi _MidiInput;
    Midi _MidiOutput;

#ifdef	__cplusplus
}
#endif

#endif	/* MIDI_H */

