#include "object.h"
//
// Created by onur on 10.06.2018.
//
typedef struct native_fnc_t {
    z_reg_t *(*fnc)(z_reg_t *, z_reg_t *, z_object_t*);
} native_fnc_t;


map_t *native_functions = NULL;

z_reg_t *native_strlen(z_reg_t *stack, z_reg_t *return_reg, z_object_t* str) {
    return_reg->type = TYPE_NUMBER;
    return_reg->number_val = strlen(str->operations.to_string(str));
    return stack;
}

z_reg_t *native_object_new(z_reg_t *stack, z_reg_t *return_reg, z_object_t* ignore) {
    return_reg->type = TYPE_OBJ;
    return_reg->val = (int_t) object_new(NULL);
    return stack;
}

z_reg_t *native_print(z_reg_t *stack, z_reg_t *return_reg, z_object_t* ignore) {
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

native_fnc_t *wrap_native_fnc(z_reg_t *(*fnc)(z_reg_t *, z_reg_t *, z_object_t*)) {
    native_fnc_t *wrapper = (native_fnc_t *) z_alloc_or_die(sizeof(native_fnc_t));
    wrapper->fnc = fnc;
    return wrapper;
}

void z_native_funcions_init() {
    native_functions = map_new(sizeof(native_fnc_t));
    native_fnc_t wrapper;
    wrapper.fnc = native_print;
    map_insert(native_functions, "print", &wrapper);
    wrapper.fnc = native_object_new;
    map_insert(native_functions, "Object", &wrapper);
}
