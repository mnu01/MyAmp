/* 
 * File:   buffer.h
 * Author: M'nu
 *
 * Created on 4 janvier 2019, 22:01
 */

#ifndef BUFFER_H
#define	BUFFER_H

#include "hardware.h"
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define BUFFER_SIZE     24
//#define BATCH_SIZE      BATCH_READ / 8

    typedef struct
    {
        bool Pages[BUFFER_SIZE][BATCH_READ];
        unsigned char iBufferRead;
        unsigned char iBufferWrite;
        unsigned char Size;
    } CircularBuffer;

    void Buffer_Reset(CircularBuffer *ABuffer);
    void Buffer_Write(bool *APage, CircularBuffer *ABuffer);
    bool Buffer_Read(bool *APage, CircularBuffer *ABuffer);

    CircularBuffer _HubBuffer;

#ifdef	__cplusplus
}
#endif

#endif	/* BUFFER_H */

