//
// Created by onur on 08.06.2018.
//
#include <stdio.h>
#include <time.h>
#include "../common/common.h"
#include "../program/instruction.h"

#define USE_DIRECT_THREADING
//#define NO_DYNAMIC_COMPILATION

#ifdef USE_DIRECT_THREADING
#include "interpreter_dd.c"
#else
#include "interpreter.c"
#endif


int main(int argc, const char *argv[]) {
    printf("----interpreter----\n");
    const char *filename = argv[1];
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    size_t fsize = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);
    char *bytes = z_alloc_or_gc(fsize + 1);
    fread(bytes, fsize, 1, f);
    fclose(f);
    clock_t begin = clock();

    object_manager_init("");
    object_manager_register_object_type((char *) filename, bytes, fsize);
    object_new((char *) filename);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time spent: %lf\n", time_spent);
}