#include "object.h"
//
// Created by onur on 10.06.2018.
//
typedef struct native_fnc_t {
    z_reg_t *(*fnc)(z_reg_t *, z_reg_t *);
} native_fnc_t;


map_t *native_functions = NULL;

z_reg_t *native_object_new(z_reg_t *stack, z_reg_t* return_reg){
    return_reg->type = TYPE_OBJ;
    return_reg->val = (int_t) object_new(NULL);
}

z_reg_t *native_print(z_reg_t *stack, z_reg_t *return_reg) {
    z_reg_t *arg = stack--;
    if (arg->type == TYPE_NUMBER) {
#ifdef FLOAT_SUPPORT
        FLOAT val = (FLOAT) arg->number_val;
        int_t ival = (int_t) val;
        if (val == ival) {
            printf("%d\n", ival);
        } else {
            printf("%f\n", val);
        }
#else
        printf("%d\n", arg->val);
#endif
    } else {
        puts(((z_object_t *) arg->val)->operations.to_string((void *) arg->val));
    }
    return stack;
}

void z_native_funcions_init() {
    native_functions = map_new(sizeof(native_fnc_t));
    native_fnc_t wrapper;
    wrapper.fnc = native_print;
    map_insert(native_functions, "print", &wrapper);
    wrapper.fnc = native_object_new;
    map_insert(native_functions, "Object", &wrapper);
}
