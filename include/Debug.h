/**
Debugging macros and defines
\author Matjaz Kovac

To enable all macros define "DEBUG 1",
to skip PRINT define "DEBUG 2".
DEBUG symbol must be defined before debug.h is included.
*/

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DEBUG
#include <stdio.h>

#if DEBUG < 2
    #define PRINT(x...) printf(x)
#else
    #define PRINT(x...) {}
#endif

    #define TRACE(x...) do { \
        printf("%s line %d, %s(): ", __FILE__, __LINE__, __FUNCTION__ ); \
        printf(x); \
    } while (0)



#else
    #define PRINT(x...) {}
    #define TRACE(x...) {}
#endif

#endif // _DEBUG_H
