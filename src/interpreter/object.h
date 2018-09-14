//
// Created by onur on 16.07.2018.
//

#ifndef ZEROSCRIPT_OBJECT_H
#define ZEROSCRIPT_OBJECT_H

typedef struct z_type_info_t {
    char *class_name;
    char *bytecode_stream;
    int_t bytecode_size;
    void* saved_state;
} z_type_info_t;

struct operations {
    char* (*to_string)(void* self);
};

typedef struct z_object {
    int_t ref_count;
    map_t *properties;
    void* key_list_cache;
    union {
        struct {
            struct z_type_info_t type_info;
            z_interpreter_state_t* saved_state;
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
            z_interpreter_state_t *responsible_interpreter_state;
        } function_ref_object;
    };
    struct operations operations;
} z_object_t;
Z_INLINE z_object_t *object_new(char *class_name);
Z_INLINE z_object_t *string_new(char *data);

#endif //ZEROSCRIPT_OBJECT_H
