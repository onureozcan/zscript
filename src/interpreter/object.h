//
// Created by onur on 16.07.2018.
//

#ifndef ZEROSCRIPT_OBJECT_H
#define ZEROSCRIPT_OBJECT_H

typedef struct z_type_info_t {
    char *class_name;
    char *bytecode_stream;
    int_t bytecode_size;
    map_t *static_variables;
    map_t *imports_table;
} z_type_info_t;

struct operations {
    char* (*to_string)(void* self);
};

typedef struct z_object {
    int_t ref_count;
    map_t *properties;
    void* key_list_cache;
    struct operations operations;
    union {
        struct {
            z_type_info_t* type_info;
            z_interpreter_state_t* saved_state;
        } ordinary_object;
        struct {
            //parent scope
            void *parent_context;
            //return context
            void *return_context;
            //we will use it to catch exceptions which are happened inside another class.
            //if return context is null but catch context is not, than we have an exception thrown inside
            //someone else's function.
            void *catch_context;
            void *locals;
            map_t *symbol_table;
            arraylist_t *catches_list;
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
        struct {
            char *value;
        } class_ref_object;
    };
} z_object_t;
Z_INLINE z_object_t *object_new(char *class_name, map_t* imports_table);
Z_INLINE z_object_t *string_new(char *data);

#endif //ZEROSCRIPT_OBJECT_H
