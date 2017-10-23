#include "persistant.h"
#include "i2c.h"
#include <xc.h>
#include <string.h>

void Persistant_Init()
{
    I2C_Init(400000); // 400 kHz
}

bool Persistant_ControlByte(int AAddress, bool ARead)
{
    if (I2C_Write(0xA0 | 0x06 & (AAddress >> 7) | ARead))
        return I2C_Ack(true);  
    return false;
}

bool Persistant_Address(int AAddress, bool ARead)
{
    if (I2C_Start())
    {
        if (Persistant_ControlByte(AAddress, ARead))
        {
            if (I2C_Write(AAddress & 0xFF))
                return I2C_Ack(true);
        }
    }
    return false;
}
  
unsigned char Persistant_ReadExternalMemory(int AAddress)
{
    if (Persistant_Address(AAddress, false))
    {
        if (I2C_Start())
        {
            if (Persistant_ControlByte(AAddress, true))
            {
                unsigned char LResult = I2C_Read();
                if (I2C_Ack(false))
                {
                    if (I2C_Stop())
                        return LResult;
                }
            }
        }
    }
    return 0x00;
}

bool Persistant_WriteExternalMemory(int AAddress, unsigned char AData)
{
    if (Persistant_Address(AAddress, false))
    {
        if (I2C_Write(AData))
        {
            if (I2C_Ack(true))
            {
                if (I2C_Stop())
                {
                    return Persistant_ReadExternalMemory(AAddress) == AData;
                }
            }
        }
    }
    return false;
}

// Read saved data
unsigned char Persistant_ReadInternalMemory(int AAddress) 
{
    EEADR = AAddress; // load address of EEPROM to read
    EECON1bits.EEPGD = 0; // access EEPROM data memory
    EECON1bits.CFGS = 0; // do not access configuration registers
    EECON1bits.RD = 1; // initiate read 
    return EEDATA; // return EEPROM byte
}

// Write Byte to internal EEPROM
bool Persistant_WriteInternalMemory(int AAddress, unsigned char AData) 
{
    EECON1bits.WREN = 1; // allow EEPROM writes
    EEADR = AAddress; // load address of write to EEPROM
    EEDATA = AData; // load data to write to EEPROM
    EECON1bits.EEPGD = 0; // access EEPROM data memory
    EECON1bits.CFGS = 0; // do not access configuration registers
    unsigned char LState = INTCONbits.GIE;
    INTCONbits.GIE = 0; // disable interrupts for critical EEPROM write sequence
    //===============//
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    //==============//
    INTCONbits.GIE = LState; // restore privious interrupts state, critical sequence complete
    while (EECON1bits.WR == 1); // wait for write to complete
    EECON1bits.WREN = 0; // do not allow EEPROM writes

    //Verify write operation
    return (bool)(Persistant_ReadInternalMemory(AAddress) == AData); // read the byte we just wrote to EEPROM
}

bool Persistant_SaveBuffer(DataBuffer ABuffer)
{
    bool LResult = true;
    ZipContainer LZip;
    LZip.Buffer = &ABuffer;
    Zip(&LZip);
    for (char i = 0; i < sizeof(LZip.Bytes); i++)
        LResult &= Persistant_WriteExternalMemory((ABuffer.Channel / 2 ) * sizeof(LZip.Bytes) + i, LZip.Bytes[i]);
    memcpy(&_Hub.BackupBuffer, &ABuffer, sizeof(DataBuffer));
    return LResult;
}

bool Persistant_LoadBuffer(DataBuffer *ABuffer)
{
    bool LResult = true;
    unsigned char LChannel = ABuffer->Channel;
    ZipContainer LZip;
    LZip.Buffer = ABuffer;
    for (char i = 0; i < sizeof(LZip.Bytes); i++)
        LZip.Bytes[i] = Persistant_ReadExternalMemory((LChannel / 2 ) * sizeof(LZip.Bytes) + i);
    UnZip(&LZip);
//    Zip(&LZip); // for testing
    ABuffer->Channel = LChannel;
    _Hub.CurrentChannel = LChannel;
    memcpy(&_Hub.BackupBuffer, ABuffer, sizeof(DataBuffer));
    return LResult;
}



