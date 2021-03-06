#include "user.h"
#include "i2c.h"

void I2C_Init(long AFrequency)
{
    TRISB0 = 0;
    TRISB1 = 0;
    
    __delay_ms(100);
    
    LATB0 = 0;
    LATB1 = 0;

    __delay_ms(100);

    TRISB0 = 1;      /* SDA and SCL as input pin */
    TRISB1 = 1;      /* these pins can be configured either as i/p or o/p */
    SSPSTAT = 0x00; // 0x80; /* Slew rate disabled */
    SSPCON1 = 0x28;  /* SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)) */
    unsigned char LRatio = (_XTAL_FREQ / (4 * AFrequency + 1));
    SSPADD = LRatio; //Setting Clock Speed
    SSPCON2 = 0x00;
    
    CKE = 0;
    SMP = 1;
    SSPIF = 0;
    BCLIF = 0;
}

bool I2C_Wait()
{
    int LTimeOut = 10000;
    while (((SSPCON2 & 0x1F ) | RW ) && LTimeOut > 0) 
    {
        __delay_ms(1);
        LTimeOut--;
    }; // wait for idle and not writing
    bool LResult = LTimeOut > 0;
    _I2CResult |= LResult ? I2CS_OK : I2CE_WAIT;
    if (LResult)
    {
        SSPIF = 0;
        _I2CCount++;
    }
    return LResult;
}

bool I2C_Start()
{
    if (I2C_Wait())
    {
        SEN = 1;          /*start condition is enabled*/
        _I2CResult |= I2CS_OK;
        _I2CCount++;
        return true;
    }
    _I2CResult |= I2CE_START;
    return false;
}

bool I2C_Restart()
{
    if (I2C_Wait())
    {
        RSEN = 1;        /* Repeated start enabled */
        _I2CResult |= I2CS_OK;
        _I2CCount++;
        return true;
    }
    _I2CResult |= I2CE_RESTART;
    return false;
}

bool I2C_Stop()
{
    if (I2C_Wait())
    {
        PEN = 1;        /* stop enable bit */ 
        _I2CResult |= I2CS_OK;
        _I2CCount++;
        return true;
    }
    _I2CResult |= I2CE_STOP;
    return false;
}

bool I2C_Ack(bool AAck)
{
    if (I2C_Wait())
    {
        ACKDT = AAck;          /*cleared for acknowledgement */    
        ACKEN = 1;          /*acknowledgment enable */
        _I2CResult |= I2CS_OK;
        _I2CCount++;
        return true;
    }
    _I2CResult |= I2CE_ACK;
    return false;
}

bool I2C_Write(unsigned b)
{
    if (I2C_Wait())
    {
        SSPBUF = b;            /*moves the data to the buffer register */
        _I2CResult |= I2CS_OK;
        _I2CCount++;  
        return !ACKSTAT;
    }
    _I2CResult |= I2CE_WRITE;
    return false;
}

unsigned short I2C_Read()
{
    if (I2C_Wait())
    {
        RCEN = 1;
        if (I2C_Wait())
        {
            unsigned short LData = SSPBUF;
            if (I2C_Ack(true))
            {
                _I2CResult |= I2CS_OK;
                _I2CCount++;
                return LData;
            }
        }
    }
    _I2CResult |= I2CE_READ;
    return 0x00;
}

