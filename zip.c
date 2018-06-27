#include "zip.h"
#include "hub.h"
#include "digipot.h"
#include <xc.h>
#include <string.h>
#include <stdbool.h>

bool Zip(ZipContainer *AZip)
{
    unsigned char *LBytes = AZip->Bytes;
    for(int i = 0; i < sizeof(AZip->Bytes); i++)
        LBytes[i] = 0x00;
    //011110 010100 001010
    unsigned char LIndex = 0;
    LBytes[0x00] = AZip->Buffer->Sound;
    for(int i = 0; i < sizeof(AZip->Buffer->Values); i++)
    {
        for(int j = 0; j < RVAR_LENGHT; j++)
        {
            LIndex = i * RVAR_LENGHT + j;
            LBytes[(char)(LIndex / 8.0f + 1.0f)] |= ((AZip->Buffer->Values[i] >> j) & 0x01) << (LIndex % 8);
        }
    }
}

bool UnZip(ZipContainer *AZip)
{
    AZip->Buffer->Sound = AZip->Bytes[0x00];
    for(int i = 0; i < DIGIPOT; i++)
    {
        char LValue = 0x00;
        for(int j = 0; j < RVAR_LENGHT; j++)
        {
            char LIndex = i * RVAR_LENGHT + j;
            LValue |= ((AZip->Bytes[(char)(LIndex / 8.0f + 1.0f)] >> (LIndex % 8)) & 0x01) << j;
        }
        AZip->Buffer->Values[i] = LValue;
    }
}
