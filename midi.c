#include "midi.h"
#include "system.h"
#include "hub.h"
#include <xc.h>
#include <stdbool.h>

void Midi_Init()
{
    Midi_ResetBuffer(&_MidiInput);
    Midi_ResetBuffer(&_MidiOutput);
    
    RCIF = 0; //reset RX pin flag
    RCIP = 0; //Not high priority
    RCIE = 1; //Enable RX interrupt
    PEIE = 1; //Enable pheripheral interrupt (serial port is a pheripheral)
    
    const unsigned long LBaudRate = 31250;
    long LPeriod;
    LPeriod = SYS_FREQ/(LBaudRate*64) - 1;   //SPBRG for Low Baud Rate
    if(LPeriod > 255)                                       //If High Baud Rage Required
    {
      LPeriod = SYS_FREQ/(LBaudRate*16) - 1; //SPBRG for High Baud Rate
      BRGH = 1;                                     //Setting High Baud Rate
    }
    if(LPeriod < 255)
    {
      SPBRG = LPeriod;  // Writing SPBRG Register
      TRISC7 = 1;       // As Prescribed in Datasheet
      TRISC6 = 1;       // As Prescribed in Datasheet
      SYNC = 0;         // Setting Asynchronous Mode, ie UART
      PIE1bits.RCIE = 1;

      SPEN = 1;         // Enables Serial Port | Rx enable
      CREN = 1;         // Enables Continuous Reception
      RX9 = 0;          // 8bits mode

      TXEN = 1;         // Enables Transmission
      TX9 = 0;
    }
}

void Midi_ResetBuffer(Midi *AMidiBuffer)
{
    AMidiBuffer->iBufferRead = 0;
    AMidiBuffer->iBufferWrite = 0;
    AMidiBuffer->iData = 0;
    AMidiBuffer->Size = 0;
    for (char i = 0; i < BUFFER_SIZE; i++)
    {
        AMidiBuffer->Buffer[i].Code = 0x00;
        AMidiBuffer->Buffer[i].Channel = 0x00;
        for (char j = 0; j < sizeof(AMidiBuffer->Buffer[i].Data); j++)
            AMidiBuffer->Buffer[i].Data[j] = 0x00;
    }
}

void Midi_Write(unsigned char AData, Midi *AMidiBuffer)
{
    if ((AData & 0x80) == 0x80)
    {
        AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].Code = AData & 0xF0;
        AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].Channel = AData & 0x0F;
        switch(AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].Code)
        {
            case 0xB0: // Control Change
                AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].DataCount = 2;
                break;
                
            case 0xC0: // Program Change 
                AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].DataCount = 1;
                break;
                
            default:
                AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].DataCount = -1;
                return;
        }
        AMidiBuffer->iData = 0;
    }
    else
    {
        AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].Data[AMidiBuffer->iData] = AData;
        AMidiBuffer->iData++;
    }
    
    if (AMidiBuffer->iData == AMidiBuffer->Buffer[AMidiBuffer->iBufferWrite].DataCount && AMidiBuffer->Size <= BUFFER_SIZE)
    {
//        LATB4 = 1;
        AMidiBuffer->Size++;
        AMidiBuffer->iBufferWrite++;
        if (AMidiBuffer->iBufferWrite >= BUFFER_SIZE)
            AMidiBuffer->iBufferWrite = 0;
    }
}

bool Midi_Read(MidiData *AData, Midi *AMidiBuffer)
{
    if (AMidiBuffer->Size > 0)
    {
        *AData = AMidiBuffer->Buffer[AMidiBuffer->iBufferRead];
        AMidiBuffer->Size--;
        AMidiBuffer->iBufferRead++;
        if (AMidiBuffer->iBufferRead >= BUFFER_SIZE)
            AMidiBuffer->iBufferRead = 0;
        return true;
    }
    else
        return false;
}


