//
// Created by onur on 11.07.2018.
//

#ifndef ZEROSCRIPT_OBJECT_C
#define ZEROSCRIPT_OBJECT_C

#include "object.h"

Z_INLINE z_object_t *string_new(char *data);

#include "types/string.h"

char* obj_to_string(void* _self){
    return "[object]";
}

static map_t *known_types_map = NULL;
struct operations string_operations = {
        str_to_string
};
struct operations object_operations = {
        obj_to_string
};

void object_manager_init() {
    known_types_map = map_new(sizeof(z_type_info_t));
}

void object_manager_register_object_type(char *class_name, char *bytecodes, int_t size) {
    z_type_info_t *type_info = (z_type_info_t *) (z_alloc_or_die(sizeof(z_type_info_t)));
    type_info->class_name = class_name;
    type_info->bytecode_stream = bytecodes;
    type_info->bytecode_size = size;
    map_insert(known_types_map, class_name, type_info);
}

Z_INLINE z_object_t *object_new(char *class_name) {
    z_object_t *obj = (z_object_t *) z_alloc_or_die(sizeof(z_object_t));
    obj->properties = map_new(sizeof(z_reg_t));
    obj->ref_count = 0;
    if (class_name) {
        obj->ordinary_object.type_info = *((z_type_info_t *) map_get(known_types_map, class_name));
        z_interpreter_run(obj->ordinary_object.type_info.bytecode_stream, obj->ordinary_object.type_info.bytecode_size);
    }
    obj->operations = object_operations;
    return obj;
}

Z_INLINE z_object_t *context_new() {
    z_object_t *obj = (z_object_t *) z_alloc_or_die(sizeof(z_object_t));
    obj->ref_count = 0;
    obj->context_object.symbol_table = NULL;
    return obj;
}

Z_INLINE z_object_t *function_ref_new(uint_t start_addr, void *parent_context) {
    z_object_t *obj = (z_object_t *) z_alloc_or_die(sizeof(z_object_t));
    obj->ref_count = 0;
    obj->function_ref_object.start_address = start_addr;
    obj->function_ref_object.parent_context = parent_context;
    return obj;
}

Z_INLINE z_object_t *string_new(char *data) {
    z_object_t *obj = (z_object_t *) z_alloc_or_die(sizeof(z_object_t));
    obj->ref_count = 0;
    obj->string_object.value = data;
    obj->operations = string_operations;
    obj->properties = map_new(sizeof(z_reg_t));
    z_reg_t length;
    length.type = TYPE_NUMBER;
    length.number_val = strlen(data);
    map_insert(obj->properties, "length", &length);
    return obj;
}

map_t *build_symbol_table(const char *data) {
    map_t *symbol_table = map_new(sizeof(int_t));
    int_t pos = 0;
    uint_t size = *((uint_t *) (data + pos));
    pos += sizeof(int_t);
    for (int_t i = 0; i < size; i++) {
        int_t value = i + 1;
        char *item = (char *) (data + pos);
        pos += strlen(item) + 1;
        map_insert(symbol_table, item, &value);
    }
    return symbol_table;
}

#endif //ZEROSCRIPT_OBJECT_C
