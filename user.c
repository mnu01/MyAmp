/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <string.h>
#include "user.h"
#include "display.h"
#include "hub.h"
#include "sequencer.h"
#include "persistant.h"
#include "digipot.h"
#include "midi.h"
#include "i2c.h"
#include "system.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    ADCON1 = 0x0F; // disable all analog port
    
    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */

    /* Enable interrupts */
    PEIE = 1;     //Enable Peripheral Interrupt
    GIE = 1;      //Enable INTs globally
    
    Persistant_Init();
    Hub_Init();
    Display_Init();
    
    bool LResult = Persistant_WriteExternalMemory(0x0001, 0x0F);
    unsigned char test = Persistant_ReadExternalMemory(0x0001);
        
    // Read channel & values from memory
//    unsigned char LBuffer[sizeof(_Config)];
//    for(int i = 0; i < sizeof(_Config); i++)
//        LBuffer[i] = Persistant_ReadInternalMemory(i);
//    memcpy(&_Config, LBuffer, sizeof(_Config));
//    _Hub.PrimaryBuffer.Channel = _Config.Channel * 2;
//    _Hub.PrimaryBuffer.Channel = 0;
//    if (test == 0x00)
    _Hub.PrimaryBuffer.Channel = test * 2;
    
    Persistant_LoadBuffer(&_Hub.PrimaryBuffer);
    Hub_CopyBuffer();
    
    Display_ProcessData(_Hub.PrimaryBuffer.Channel, _Hub.PrimaryBuffer.Values, true, _Hub.PrimaryBuffer.PushedButton);
    Sequencer_Start();
    InitDigiPot();
    Midi_Init();
}

