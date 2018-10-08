#include "object.h"
//
// Created by onur on 10.06.2018.
//
typedef struct native_fnc_t {
    z_reg_t *(*fnc)(z_reg_t *, z_reg_t *, z_object_t *);
} native_fnc_t;


map_t *native_functions = NULL;

z_reg_t *native_gc(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    gc();
    return stack;
}

z_reg_t *native_number(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    z_reg_t *arg = stack--;
    if (arg->type == TYPE_NUMBER) {
        return_reg->number_val = arg->number_val;
    } else {
        return_reg->number_val = (FLOAT)(atof(((z_object_t*)arg->val)->operations.to_string((void *) arg->val)));
    }
    return_reg->type = TYPE_NUMBER;
    return stack;
}

z_reg_t *native_strlen(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    return_reg->type = TYPE_NUMBER;
    return_reg->number_val = strlen(str->operations.to_string(str));
    return stack;
}

z_reg_t *native_object_new(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    return_reg->type = TYPE_OBJ;
    return_reg->val = (int_t) object_new(NULL,NULL);
    return stack;
}

z_reg_t *native_object_key_size(z_reg_t *stack, z_reg_t *return_reg, z_object_t *object) {
    return_reg->number_val = object->properties->size;
    return_reg->type = TYPE_NUMBER;
    return stack;
}

z_reg_t *native_object_key_list(z_reg_t *stack, z_reg_t *return_reg, z_object_t *object) {
    return_reg->type = TYPE_OBJ;
    z_object_t *ret = (z_object_t *) object->key_list_cache;
    if (!ret) {
        ret = object_new(NULL, NULL);
        arraylist_t *keys = map_key_list(object->properties);
        for (int_t i = 0; i < keys->size; i++) {
            char *value = *(char **) arraylist_get(keys, i);
            z_reg_t value_reg;
            value_reg.val = (int_t) string_new(value);
            value_reg.type = TYPE_STR;
            map_insert(ret->properties, num_to_str(i), &value_reg);
        }
        object->key_list_cache = ret;
        ret->properties->is_immutable = 1;
    }
    return_reg->val = (int_t) ret;
    return stack;
}

z_reg_t *native_print(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
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

z_reg_t *native_to_int(z_reg_t *stack, z_reg_t *return_reg, z_object_t *object) {
    z_reg_t *arg = stack--;
    return_reg->number_val = ((int_t)arg->number_val);
    return_reg->type = TYPE_NUMBER;
    return stack;
}

native_fnc_t *wrap_native_fnc(z_reg_t *(*fnc)(z_reg_t *, z_reg_t *, z_object_t *)) {
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
    wrapper.fnc = native_number;
    map_insert(native_functions, "number", &wrapper);
    wrapper.fnc = native_to_int;
    map_insert(native_functions, "toInt", &wrapper);
    wrapper.fnc = native_gc;
    map_insert(native_functions, "gc", &wrapper);
}
