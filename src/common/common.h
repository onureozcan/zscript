//
// Created by onur on 11.05.2018.
//

#ifndef ZEROSCRIPT_COMMON_H
#define ZEROSCRIPT_COMMON_H

#define Z_INLINE inline
#define Z_MALLOC malloc
#define Z_REALLOC realloc
#define Z_FREE free
#define FLOAT_SUPPORT
#define TRUE 1
#define FALSE 0
//#define PRINT_LOGS

void* z_log(const char *format, ...);

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "types.h"
#include "error.c"
#include "alloc.c"

#include "data_structures/arraylist.c"
#include "data_structures/map.c"

arraylist_t* thread_list;

#ifdef PRINT_LOGS

void* z_log(const char *format, ...)
{
    char buff[500];
    snprintf(buff,499,"[THREAD:%p]%s", (void *)(pthread_self()),format);
    va_list args;
    va_start(args, format);
    vfprintf(stderr,buff, args);
    va_end(args);
}

#else
void* z_log(const char *format, ...){

}
#endif

#endif //ZEROSCRIPT_COMMON_H
