//
// Created by onur on 16.07.2018.
//

#ifndef ZEROSCRIPT_OBJECT_H
#define ZEROSCRIPT_OBJECT_H

typedef struct z_type_info_t {
    char *class_name;
    char *bytecode_stream;
    map_t *symbol_table;
    int_t bytecode_size;
} z_type_info_t;

struct operations {
    char* (*to_string)(void* self);
};

typedef struct z_object {
    int_t ref_count;
    map_t *properties;
    union {
        struct {
            struct z_type_info_t type_info;
        } ordinary_object;
        struct {
            void *parent_context;
            void *return_context;
            void *locals;
            map_t *symbol_table;
            uint_t symbols_address;
            uint_t requested_return_register_index;
            z_instruction_t *return_address;
        } context_object;
        struct {
            char *value;
        } string_object;
        struct {
            uint_t start_address;
            void *parent_context;
        } function_ref_object;
    };
    struct operations operations;
} z_object_t;
Z_INLINE z_object_t *object_new(char *class_name);

#endif //ZEROSCRIPT_OBJECT_H
