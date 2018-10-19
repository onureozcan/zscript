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

#endif //ZEROSCRIPT_COMMON_H
