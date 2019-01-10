
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "display.h"
#include "hub.h"
#include "persistant.h"
#include "sequencer.h"
#include "digipot.h"
#include "switch.h"
#include "midi.h"
#include "framework/usb/inc/usb.h"
#include "framework/usb/inc/usb_device.h"
#include "framework/usb/inc/usb_device_cdc.h"
#include <math.h>
#include <string.h>

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

bool _Modified = false;
bool _Saving = false;
void Sequencer_TimerEvent()
{
    if (_Saving)
        _Display.SavingMask = ~_Display.SavingMask;
    if (_Modified && (CHANNEL_DIGIT > 1))
        _Display.PointMask[1] = ~_Display.PointMask[1];
}

unsigned char iUpdate = 0;
void interrupt Interrupt(void)
{
    if (TMR0IF)
    {
        if (_Display.Counter >= _Display.CounterRef)
        {
            WriteData(_Display.PrimaryBuffer + _Display.iDisplay * (CHANNEL_DIGIT + DIGIPOT), CHANNEL_DIGIT + DIGIPOT, _Display.IO);
            _Display.iDisplay++;
            if (_Display.iDisplay >= DIVISION) //sizeof(_Convert))
            {
    //            for (int i = 0; i < DIVISION; i++)
    //                DisplayData(_Display.PrimaryBuffer + (_Display.iDisplay - 1) * (CHANNEL_DIGIT + DIGIPOT), CHANNEL_DIGIT + DIGIPOT);

                if (_Display.ReadyToRead)
                    Display_SwapBuffer();
                _Display.iDisplay = 0;
            }
            _Display.Counter = 0;
        }
         
        if (_Hub.Counter >= _Hub.CounterRef)
        {
            Hub_ReadSwitch();
            _Hub.Counter = 0;
        }
        
        _DigiPot.Counter++;
        _Switch.Counter++;
        _Hub.Counter++;
        _Display.Counter++;
        
        TMR0IF = 0;               
    }
    else if (TMR1IF)
    {
        TMR1IF = 0;
        _Timer.Counter++;
        if (_Timer.Counter >= _Timer.Ticks)
        {
            _Timer.Counter = 0;
            _Timer.Elapsed = true;
            Sequencer_TimerEvent();
            if (_Timer.Loop)
                _Timer.Elapsed = false;
            else
                Sequencer_StopTimer();
        }
    }
    else if (PIR1bits.RCIF)
    {
//        PIR1bits.RCIF = 0;
//        LATB4 = 0;
        unsigned char LData = 0x00;
        while (PIR1bits.RCIF)
        {
            LData = RCREG;
            if (RCSTAbits.FERR || RCSTAbits.OERR)
            {
                Midi_ResetBuffer(&_MidiInput);
                CREN = 0;
                CREN = 1;
            }
            else
                Midi_Write(LData, &_MidiInput);
        }
    }
}

void Load(unsigned char AChannel)
{
    _Hub.PrimaryBuffer.Channel = AChannel;

    // Load data from memory to hub primary buffer
    Persistant_LoadBuffer(&_Hub.PrimaryBuffer);
    _Config.Channel = AChannel / 2;
    unsigned char LBuffer[sizeof(_Config)];
    memcpy(LBuffer, &_Config, sizeof(_Config));
    for(int i = 0; i < sizeof(_Config); i++)
        Persistant_WriteInternalMemory(i, LBuffer); // save current channel

    for (int i = 0; i < CHANNEL_DIGIT; i++)
        _Display.PointMask[i] = DIGIT_POINT;
    _Modified = false;
    
    // Force immediate update output
    _DigiPot.Counter = _DigiPot.CounterRef;
    _Switch.Counter = _Switch.CounterRef;
}

static uint8_t _USBBufferIn[1];
static uint8_t _USBBufferOut[1];
void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();
    
    USBDeviceInit();
    USBDeviceAttach();
    
    bool LEncSwitchPushed = false;
    DataBuffer LPreviousBuffer;
    memcpy(&LPreviousBuffer, &_Hub.PrimaryBuffer, sizeof(DataBuffer));
    while(1)
    {
        #if defined(USB_POLLING)
            USBDeviceTasks();
        #endif

        if( USBGetDeviceState() >= CONFIGURED_STATE )
        {
            if( USBUSARTIsTxTrfReady() == true)
            {
                if (getsUSBUSART(_USBBufferIn, 1) > 0)
                {
                    Midi_Write(_USBBufferIn, &_MidiInput);
                    Load(_USBBufferIn[0] * 2);
//                    _USBBufferOut[0] = 31;
//                    putUSBUSART(_USBBufferOut, 1);
                }
            }            
        }
            
        bool LIsBufferFull = (_HubBuffer.Size == BUFFER_SIZE);

        Hub_UpdateValues();
        
        if (!_Hub.PrimaryBuffer.PushedButton) 
        {
            if (_Hub.PrimaryBuffer.ActionButton != LPreviousBuffer.ActionButton)
            {
                switch (_Hub.PrimaryBuffer.ActionButton)
                {
                    case 0:
                        Sequencer_StopTimer();
                        if (_Saving)
                        {
                            // If in saving mode, backup data to internal memory
                            Persistant_SaveBuffer(_Hub.PrimaryBuffer);

                            // Reset timer & out saving mode
                            _Modified = false;
                            _Saving = false;
                            _Display.SavingMask = 0xFF;
                        }
                        else
                        {
                            // Start a 2s timer before enterring saving mode
                            LEncSwitchPushed = true;
                            Sequencer_StartTimer(2.0f, false);
                        }
                        break;
                    case 1:
                        LEncSwitchPushed = false;
                        if (!_Saving && !_Timer.Elapsed)
                        {
                            // in loading mode, read data from internal memory & save current channel
                            Sequencer_StopTimer();
                            Load(_Hub.PrimaryBuffer.Channel);
                        }
                        break;
                }
            }
            else
            {
                // on channel switch pushed & time elapse, enterring in saving mode
                if (LEncSwitchPushed && _Timer.Elapsed && !_Saving)
                {
                    Sequencer_StopTimer();
                    _Saving = true;
                    Sequencer_StartTimer(0.3f, true);
                }
            }
        }
        
        MidiData LMidiData;
        if (Midi_Read(&LMidiData, &_MidiInput))
        {
            switch(LMidiData.Code)
            {
                case 0xC0:
                    Load(LMidiData.Data[0] * 2);
                    break;
                case 0xB0:
                {
                    unsigned char LValue = LMidiData.Data[1] / 2;
                    switch(LMidiData.Data[0])
                    {
                        case 0x07: // volume
                            _Hub.PrimaryBuffer.Values[VOLUME_INDEX] = LValue;
                            break;
                        case 0x4B: // Gain
                            _Hub.PrimaryBuffer.Values[GAIN_INDEX] = LValue;
                            break;
                        case 0x4C: // Bass
                            _Hub.PrimaryBuffer.Values[BASS_INDEX] = LValue;
                            break;
                        case 0x4D: // Middle
                            _Hub.PrimaryBuffer.Values[MIDDLE_INDEX] = LValue;
                            break;
                        case 0x4E: // Treble
                            _Hub.PrimaryBuffer.Values[TREBLE_INDEX] = LValue;
                            break;
                    }
                    break;
                }
            }
        }

//        char LTest1[sizeof(DataBuffer)];
//        char LTest2[sizeof(DataBuffer)];
//        memcpy(LTest1, &_Hub.SecondaryBuffer, sizeof(DataBuffer));
//        memcpy(LTest2, &_Hub.BackupBuffer, sizeof(DataBuffer));
        bool LHasChanged = memcmp(_Hub.PrimaryBuffer.Values, _Hub.BackupBuffer.Values, sizeof(_Hub.PrimaryBuffer.Values)) != 0 || _Hub.BackupBuffer.Sound != _Hub.PrimaryBuffer.Sound;
        
        if (!_Modified && (CHANNEL_DIGIT > 1) && LHasChanged)
        {
            Sequencer_StopTimer();
            _Modified = true;
            _Display.PointMask[1] = DIGIT_POINT;
            Sequencer_StartTimer(0.3f, true);
        }
        
        if (CHANNEL_DIGIT > 0 && (_Hub.PrimaryBuffer.PushedButton || LIsBufferFull))
            _Display.PointMask[0] = DIGIT_POINT;
        
        Display_ProcessData(_Hub.PrimaryBuffer.PushedButton && !_Saving ? _Hub.PrimaryBuffer.Sound : _Hub.PrimaryBuffer.Channel, _Hub.PrimaryBuffer.Values, _Hub.CurrentChannel == _Hub.PrimaryBuffer.Channel, _Hub.PrimaryBuffer.PushedButton);
        
        if (_DigiPot.Counter >= _DigiPot.CounterRef)
        {
            if (memcmp(_Hub.PrimaryBuffer.Values, LPreviousBuffer.Values, sizeof(_Hub.PrimaryBuffer.Values)) != 0)
            {
                WriteData(_Hub.PrimaryBuffer.Values, RVAR, _DigiPot.IO);
                _DigiPot.Counter = 0;                
            }           
        }
        
        if (_Switch.Counter >= _Switch.CounterRef)
        {
            if (_Hub.PrimaryBuffer.Sound != LPreviousBuffer.Sound)
            {
                WriteData(_Switch.Sound[_Hub.PrimaryBuffer.Sound / 2].Config, HUB_REG, _Switch.IO);
                _Switch.Counter = 0;
            }
        }
        
        memcpy(&LPreviousBuffer, &_Hub.PrimaryBuffer, sizeof(DataBuffer));
    }
}