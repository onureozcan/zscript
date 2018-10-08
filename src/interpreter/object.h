//
// Created by onur on 16.07.2018.
//

#ifndef ZEROSCRIPT_OBJECT_H
#define ZEROSCRIPT_OBJECT_H

typedef struct z_type_info_t {
    char *bytecode_stream;
    int_t bytecode_size;
    map_t *static_variables;
    map_t *imports_table;
} z_type_info_t;

struct operations {
    char* (*to_string)(void* self);
};

typedef struct z_object {
    uhalf_int_t gc_version;
    uhalf_int_t type;
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
            void *locals;
            uint_t locals_count;
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
            uint_t is_async;
            void *parent_context;
            z_interpreter_state_t *responsible_interpreter_state;
        } function_ref_object;
        struct {
            char *value;
        } class_ref_object;
    };
} z_object_t;

map_t* string_native_properties_map = 0;
arraylist_t* gc_objects_list = 0;
arraylist_t* interpreter_states_list = 0;

Z_INLINE z_object_t *object_new(char *class_name, map_t* imports_table);
Z_INLINE z_object_t *string_new(char *data);

#endif //ZEROSCRIPT_OBJECT_H
