#include "object.h"

//
// Created by onur on 10.06.2018.
//
typedef struct z_native_return_value (*z_native_fnc_t)(z_reg_t *, z_reg_t *, z_object_t *);

#define RETURN_NATIVE(s, e) return (z_native_return_value) {s,e};

map_t *native_functions = NULL;

void z_bind_native_function(char *function_name, z_native_fnc_t fnc) {
    map_insert(native_functions, function_name, &fnc);
}

void z_bind_native_string_function(char *function_name, z_native_fnc_t fnc) {
    if (!string_native_properties_map) {
        string_native_properties_map = map_new(sizeof(z_reg_t));
    }
    z_reg_t temp;
    temp.type = TYPE_NATIVE_FUNC;
    temp.val = (int_t) fnc;
    map_insert_non_enumerable(string_native_properties_map, function_name, &temp);
}

z_native_return_value native_exit(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    exit((int) (stack--)->val);
}

z_native_return_value native_gc(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    if (used_heap > heap_limit) {
        schedule_gc();
    }
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_enqueue(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    if (!event_queue) {
        event_queue = arraylist_new(sizeof(int_t));
    }
    z_reg_t *arg = stack--;
    enqueue_event(&arg->val);
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_number(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    z_reg_t *arg = stack--;
    if (arg->type == TYPE_NUMBER) {
        return_reg->number_val = arg->number_val;
    } else {
        return_reg->number_val = (FLOAT) (atof(((z_object_t *) arg->val)->operations.to_string((void *) arg->val)));
    }
    return_reg->type = TYPE_NUMBER;
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_strlen(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    return_reg->type = TYPE_NUMBER;
    return_reg->number_val = strlen(str->operations.to_string(str));
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_assert(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    z_reg_t *arg = stack--;
    if(arg->type == TYPE_NUMBER && arg->number_val == 1){
        RETURN_NATIVE(stack, NULL);
    } else
        RETURN_NATIVE(stack, "assertion failed");
}

z_native_return_value native_str_equals(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    z_reg_t *arg = stack--;
    char *other = 0;
    int_t ret = 0;
    if (arg->type != TYPE_NUMBER) {
        other = ((z_object_t *) arg->val)->operations.to_string((void *) arg->val);
        ret = strcmp(other, str->operations.to_string(str)) == 0;
    }
    return_reg->type = TYPE_NUMBER;
    return_reg->number_val = ret;
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_str_startswith(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    z_reg_t *arg = stack--;
    char *other = 0;
    int_t ret = 0;
    if (arg->type != TYPE_NUMBER) {
        other = ((z_object_t *) arg->val)->operations.to_string((void *) arg->val);
        char *this_str = str->operations.to_string(str);
        ret = strncmp(other, this_str, strlen(other)) == 0;
    }
    return_reg->type = TYPE_NUMBER;
    return_reg->number_val = ret;
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_str_substring(z_reg_t *stack, z_reg_t *return_reg, z_object_t *str) {
    z_reg_t *arg = stack--;
    char *this_str = str->operations.to_string(str);
    if (arg->type == TYPE_NUMBER) {
        char *new_str = (this_str + (int_t) arg->number_val);
        char *copied = (char *) z_alloc_or_die(strlen(new_str) + 1);
        strcpy(copied, new_str);
        return_reg->val = (int_t) string_new(copied);
        return_reg->type = TYPE_STR;
        ADD_OBJECT_TO_GC_LIST(return_reg);
        RETURN_NATIVE(stack, NULL);
    }
    return_reg->type = TYPE_NUMBER;
    return_reg->number_val = 0;
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_object_new(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    return_reg->val = (int_t) object_new(NULL, NULL, NULL, NULL, NULL);
    return_reg->type = TYPE_OBJ;
    ADD_OBJECT_TO_GC_LIST(return_reg->val);
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_object_key_size(z_reg_t *stack, z_reg_t *return_reg, z_object_t *object) {
    return_reg->number_val = object->properties->size;
    return_reg->type = TYPE_NUMBER;
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_object_key_list(z_reg_t *stack, z_reg_t *return_reg, z_object_t *object) {
    z_object_t *ret = (z_object_t *) object->key_list_cache;
    if (!ret) {
        ret = object_new(NULL, NULL, NULL, NULL, NULL);
        arraylist_t *keys = map_key_list(object->properties);
        for (int_t i = 0; i < keys->size; i++) {
            char *value = *(char **) arraylist_get(keys, i);
            char *copied = (char *) z_alloc_or_gc(strlen(value) + 1);
            strcpy(copied, value);
            z_reg_t value_reg;
            value_reg.val = (int_t) string_new(copied);
            value_reg.type = TYPE_STR;
            map_insert(ret->properties, num_to_str(i), &value_reg);
            ADD_OBJECT_TO_GC_LIST(value_reg.val);
        }
        object->key_list_cache = ret;
        ret->properties->is_immutable = 1;
    }
    return_reg->val = (int_t) ret;
    return_reg->type = TYPE_OBJ;
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_readln(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    char c;
    size_t length = 0;
    // for decoration
    char *buff = (char *) Z_MALLOC(sizeof(uint_t));
    // extremely inefficient but who cares
    while ((c = getchar()) != -1 && c != '\n') {
        length++;
        buff = (char *) Z_REALLOC(buff, length + sizeof(uint_t));
        buff[length - 1 + sizeof(uint_t)] = c;
    }
    USED_HEAP_LOCK
    used_heap += length + sizeof(uint_t);
    USED_HEAP_UNLOCK
    buff[length + sizeof(uint_t)] = 0;
    return_reg->val = (int_t) string_new((char *) z_decorate_ptr(buff, length));
    return_reg->type = TYPE_STR;
    ADD_OBJECT_TO_GC_LIST(return_reg->val);
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_print(z_reg_t *stack, z_reg_t *return_reg, z_object_t *ignore) {
    z_reg_t *arg = stack--;
    switch (arg->type) {
        case TYPE_NUMBER: {
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
            break;
        }
        case TYPE_STR: {
            puts(((z_object_t *) arg->val)->string_object.value);
            break;
        }
        default:
            puts(((z_object_t *) arg->val)->operations.to_string((void *) arg->val));
    }
    RETURN_NATIVE(stack, NULL);
}

z_native_return_value native_to_int(z_reg_t *stack, z_reg_t *return_reg, z_object_t *object) {
    z_reg_t *arg = stack--;
    return_reg->number_val = ((int_t) arg->number_val);
    return_reg->type = TYPE_NUMBER;
    RETURN_NATIVE(stack, NULL);
}


void z_native_funcions_init() {
    native_functions = map_new(sizeof(z_native_fnc_t));

    z_bind_native_function("print", native_print);
    z_bind_native_function("Object", native_object_new);
    z_bind_native_function("number", native_number);
    z_bind_native_function("int", native_to_int);
    z_bind_native_function("gc", native_gc);
    z_bind_native_function("assert", native_assert);
    z_bind_native_function("exit", native_exit);
    z_bind_native_function("read", native_readln);

    z_bind_native_string_function("length", native_strlen);
    z_bind_native_string_function("startsWith", native_str_startswith);
    z_bind_native_string_function("equals", native_str_equals);
    z_bind_native_string_function("substring", native_str_substring);

}
