//
// Created by onur on 11.05.2018.
//

#ifndef ZEROSCRIPT_TYPES_H
#define ZEROSCRIPT_TYPES_H

//integral types size
//#define MODE_32_BITS

#include <stdint-gcc.h>

typedef void* any_ptr_t;

#ifdef MODE_32_BITS
typedef uint32_t uint_t;
typedef int32_t int_t;
typedef uint16_t uhalf_int_t;
typedef int16_t half_int_t;
typedef uint8_t uquarter_int_t;
typedef int8_t quarter_int_t;
typedef float FLOAT;
#else
typedef uint64_t uint_t;
typedef int64_t int_t;
typedef uint32_t uhalf_int_t;
typedef int32_t half_int_t;
typedef uint16_t uquarter_int_t;
typedef int16_t quarter_int_t;
#ifdef FLOAT_SUPPORT
typedef float FLOAT;
#else
typedef int_t FLOAT;
#endif
#endif

#endif //ZEROSCRIPT_TYPES_H
