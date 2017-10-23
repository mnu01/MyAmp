/* 
 * File:   i2c.h
 * Author: M'nu
 *
 * Created on 20 septembre 2017, 22:02
 */

#ifndef I2C_H
#define	I2C_H

#include "hardware.h"
#include "system.h"
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define I2CS_OK          0
#define I2CE_START       1
#define I2CE_WAIT        2
#define I2CE_RESTART     4
#define I2CE_STOP        8
#define I2CE_ACK         16
#define I2CE_WRITE       32
#define I2CE_READ        64
    
    void I2C_Init(long AFrequency);
    bool I2C_Wait();
    bool I2C_Start();
    bool I2C_Restart();
    bool I2C_Stop();
    bool I2C_Ack(bool AAck);
    bool I2C_Write(unsigned char b);
    unsigned char I2C_Read();
    
    int _I2CResult = 0;
    int _I2CCount = 0;
    
#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

