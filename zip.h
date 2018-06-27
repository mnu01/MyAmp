/* 
 * File:   zip.h
 * Author: M'nu
 *
 * Created on 12 septembre 2017, 22:12
 */

#ifndef ZIP_H
#define	ZIP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "hub.h"
#include "hardware.h"
#include <stdbool.h>
    
#define ZIP_LENGTH      (char)((DIGIPOT * RVAR_LENGHT * 1.0f + 1.0f * 8.0f) / 8.0f)

    typedef struct
    {
        DataBuffer *Buffer;
        unsigned char Bytes[ZIP_LENGTH];
    }ZipContainer;

    bool Zip(ZipContainer *AZip);
    bool UnZip(ZipContainer *AZip);

#ifdef	__cplusplus
}
#endif

#endif	/* ZIP_H */

