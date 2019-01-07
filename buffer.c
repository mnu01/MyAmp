#include "buffer.h"
#include "hardware.h"
#include <stdbool.h>
#include <string.h>

void Buffer_Reset(CircularBuffer *ABuffer)
{
    ABuffer->iBufferRead = 0;
    ABuffer->iBufferWrite = 0;
    ABuffer->Size = 0;
    for (char i = 0; i < BUFFER_SIZE; i++)
    {
        for (char j = 0; j < BATCH_READ; j++)
            ABuffer->Pages[i][j] = false;
    }    
}

void Buffer_Write(bool *APage, CircularBuffer *ABuffer)
{
    memcpy(ABuffer->Pages[ABuffer->iBufferWrite], APage, BATCH_READ);
    
    if (ABuffer->Size < BUFFER_SIZE)
    {
        ABuffer->Size++;
        ABuffer->iBufferWrite++;
        if (ABuffer->iBufferWrite >= BUFFER_SIZE)
            ABuffer->iBufferWrite = 0;
    }    
}
bool Buffer_Read(bool *APage, CircularBuffer *ABuffer)
{
    if (ABuffer->Size > 0)
    {
        memcpy(APage, ABuffer->Pages[ABuffer->iBufferRead], BATCH_READ);
        ABuffer->Size--;
        ABuffer->iBufferRead++;
        if (ABuffer->iBufferRead >= BUFFER_SIZE)
            ABuffer->iBufferRead = 0;
        return true;
    }
    else
        return false;
}

