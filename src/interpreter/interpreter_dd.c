//
// Created by onur on 06.06.2018.
//

#define TYPE_NUMBER 0
#define TYPE_STR 1
#define TYPE_FUNCTION_REF 4
#define TYPE_OBJ 16
#define TYPE_NATIVE_FUNC 32
#define TYPE_INSTANCE 64
#define TYPE_CLASS_REF 128
#define TYPE_CONTEXT 256

#define INIT_R0 z_reg_t* r0 = locals_ptr + instruction_ptr->r0
#define INIT_R1 z_reg_t* r1 = locals_ptr + instruction_ptr->r1
#define INIT_R2 z_reg_t* r2 = locals_ptr + instruction_ptr->r2

#define GOTO_NEXT goto *(++instruction_ptr)->opcode;
#define GOTO_CURRENT goto *(instruction_ptr)->opcode;

#define ADD_OBJECT_TO_GC_LIST(c) GC_LIST_LOCK arraylist_push(gc_objects_list,&(c)); GC_LIST_UNLOCK
#define ADD_ROOT_TO_GC_LIST(c) GC_LIST_LOCK arraylist_push(interpreter_states_list,&(c)); GC_LIST_UNLOCK
#define REMOVE_ROOT_FROM_GC_ROOT_LIST(c) (c)->removed_as_a_root = 1;
#define CREATE_STACK(s) (z_reg_t *) (z_alloc_or_die((s) * sizeof(z_reg_t)));

pthread_mutex_t symbol_table_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_SYMBOL_TABLE pthread_mutex_lock(&symbol_table_lock);
#define UNLOCK_SYMBOL_TABLE pthread_mutex_unlock(&symbol_table_lock);


pthread_mutex_t synchronized_access_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_SYNCHRONIZED_ACCESS pthread_mutex_lock(&synchronized_access_lock);
#define UNLOCK_SYNCHRONIZED_ACCESS pthread_mutex_unlock(&synchronized_access_lock);

pthread_mutex_t interpreter_is_running_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_INTERPRETER_IS_RUNNING pthread_mutex_lock(&interpreter_is_running_lock);
#define UNLOCK_INTERPRETER_IS_RUNNING pthread_mutex_unlock(&interpreter_is_running_lock);


typedef struct z_reg_t {
    int_t type;
    union {
        int_t val;
#ifdef FLOAT_SUPPORT
        FLOAT number_val;
#else
        int_t number_val;
#endif
    };
} z_reg_t;

typedef struct z_native_return_value {
    z_reg_t *stack_ptr;
    char *error_message;
} z_native_return_value;

char *num_to_str(FLOAT val);

Z_INLINE static char *strconcat(const char *s1, const char *s2);


typedef struct z_interpreter_state_t {
    //context of currently running function
    void *current_context;
    // root context
    void *root_context;
    // compiled bytecodes
    char *byte_stream;
    // class name
    char *class_name;
    // exception message (if so)
    char *exception_details;
    // synchronized variables. access to them is protected by mutexes.
    map_t *synchronized_variables;
    int_t return_code;
    // byte code file size
    int_t fsize;
    // current instruction pointer
    int_t instruction_pointer;
    z_reg_t return_value;
    z_reg_t *stack_ptr;
    z_reg_t *stack_start;
    half_int_t is_running;
    half_int_t removed_as_a_root;
} z_interpreter_state_t;

z_interpreter_state_t *z_interpreter_run(z_interpreter_state_t *initial_state);

z_interpreter_state_t *
interpreter_state_new(void *current_context, char *bytes, int_t len, char *class_name, z_reg_t *stack_ptr,
                      z_reg_t *stack_start);

void interpreter_run_static_constructor(char *byte_stream, uint_t len, char *class_name);

z_reg_t *interpreter_get_field_virtual(z_interpreter_state_t *saved_state, z_interpreter_state_t *saved_state2,
                                       char *field_name_to_get);

int_t interpreter_set_field_virtual(z_interpreter_state_t *saved_state, z_interpreter_state_t *saved_state2,
                                    char *field_name_to_set, z_reg_t *value);

map_t *get_imports_table(z_interpreter_state_t *initial_state);

void interpreter_throw_exception_from_reg(z_interpreter_state_t *current_state, z_reg_t *object_thrown);

void interpreter_throw_exception_from_str(z_interpreter_state_t *current_state, char *message);

#include <cmath>
#include <unistd.h>
#include "object.h"
#include "event_queue.c"
#include "native_functions.c"
void z_interpreter_set_arguments_count(z_interpreter_state_t *initial_state, int_t arguments_count);
#include "object.c"

Z_INLINE char *num_to_str(
#ifdef FLOAT_SUPPORT
        FLOAT val
#else
        int_t val
#endif
) {
    char *buff = (char *) z_alloc_or_die(11);
#ifdef FLOAT_SUPPORT
    int_t ival = (int_t) val;
    if (ival != val) {
        snprintf(buff, 11, "%f", val);
    } else {
        snprintf(buff, 11, "%d", ival);
    }
#else
    snprintf(buff, 11, "%d", val);
#endif
    return buff;
}

#define GOTO_CATCH goto_catch_block(initial_state, byte_stream, &current_context, &instruction_ptr, &locals_ptr); GOTO_CURRENT;
#define RETURN_IF_ERROR if(initial_state->return_code) goto end;

Z_INLINE static char *strconcat(const char *s1, const char *s2) {
    uint_t l1 = strlen(s1);
    uint_t l2 = strlen(s2) + 1;
    char *buff = (char *) z_alloc_or_die((size_t) (l1 + l2));
    memcpy(buff, s1, l1);
    memcpy(buff + l1, s2, l2);
    return buff;
}

typedef struct z_async_fnc_args_t {
    z_interpreter_state_t *initial_state;
    z_reg_t *stack_start;
    z_reg_t *stack_ptr;
    z_object_t *function_ref;
} z_async_fnc_args_t;

#define stack_file_size 1000

void goto_catch_block(const z_interpreter_state_t *initial_state,
                      const char *byte_stream,
                      z_object_t **current_context_ptr_ptr,
                      z_instruction_t **instruction_ptr_ptr,
                      z_reg_t **locals_ptr);

void *run_async(void *argsv);

void z_thread_gc_safe_end_thread();

z_interpreter_state_t *clone_state(z_interpreter_state_t *pState);

void *run_async(void *argsv) {
    z_async_fnc_args_t *args = (z_async_fnc_args_t *) (argsv);
    z_interpreter_state_t *initial_state = args->initial_state;
    z_object_t *function_ref = args->function_ref;
    z_object_t *called_fnc = context_new();
    z_interpreter_state_t *other_state = interpreter_state_new(called_fnc, initial_state->byte_stream,
                                                               initial_state->fsize, initial_state->class_name,
                                                               args->stack_ptr,
                                                               args->stack_start);
    // inherit thread shared variables from parent state
    map_free(other_state->synchronized_variables);
    other_state->synchronized_variables = initial_state->synchronized_variables;
    ADD_ROOT_TO_GC_LIST(other_state);
    called_fnc->context_object.parent_context = function_ref->function_ref_object.parent_context;
    other_state->instruction_pointer = (function_ref->function_ref_object.start_address);
    other_state->current_context = called_fnc;
    z_interpreter_run(other_state);
    z_free(argsv);
    if (other_state->return_code != 0) {
        error_and_exit(other_state->exception_details);
    }
    z_thread_gc_safe_end_thread();
    return NULL;
}

void z_thread_gc_safe_start_thread(pthread_t *thread, void *args) {
    z_log("trying to add thread\n");
    THREAD_LIST_LOCK
    total_thread_count++;
    THREAD_LIST_UNLOCK
    schedule_gc();
    THREAD_LIST_LOCK
    z_log("added new thread, count : %d\n", total_thread_count);
    arraylist_push(thread_list, &thread);
    pthread_create(thread, NULL, run_async, args);
    THREAD_LIST_UNLOCK
}

void z_thread_gc_safe_end_thread() {
    z_log("trying to end thread\n");
    THREAD_LIST_LOCK
    total_thread_count--;
    THREAD_LIST_UNLOCK
    schedule_gc();
    z_log("quiting, remaining thread count: %d\n", total_thread_count);
}

/**
 * heart of the interpreter. interprets a given bytecode.
 * @param initial_state state representation with given parameters.
 * @return modified state.
 */
z_interpreter_state_t *z_interpreter_run(z_interpreter_state_t *initial_state) {
    LOCK_INTERPRETER_IS_RUNNING
    initial_state->is_running = 1;
    UNLOCK_INTERPRETER_IS_RUNNING

    char *byte_stream = (char *) z_alloc_or_die((size_t) initial_state->fsize);
    memcpy(byte_stream, initial_state->byte_stream, (size_t) initial_state->fsize);
    int_t fsize = initial_state->fsize;
    z_object_t *current_context = (z_object_t *) initial_state->current_context;

    char *field_name_to_get;

    z_object_t *object_to_search_on = NULL;
    z_reg_t *locals_ptr = NULL;
    z_instruction_t *instruction_ptr;

    uint_t code_start = ((uint_t *) byte_stream)[0];
    char *data = byte_stream;
    char *code = byte_stream + code_start;

    static void *dispatch_table[] = {
            &&OP_MOV_NUMBER,
            &&OP_MOV_STR,
            &&OP_MOV,
            &&OP_ADD,
            &&OP_SUB,
            &&OP_DIV,
            &&OP_MUL,
            &&OP_MOD,
            &&OP_CMP_EQUAL,
            &&OP_CMP_LESS,
            &&OP_CMP_LESS_OR_EQUAL,
            &&OP_CMP_GREATER,
            &&OP_CMP_N_EQUAL,
            &&OP_CMP_GREATER_OR_EQUAL,
            &&OP_JMP_TRUE,
            &&OP_JMP_NOT_TRUE,
            &&OP_JMP,
            &&OP_GET_FIELD,
            &&OP_GET_FIELD_IMMEDIATE,
            &&OP_SET_FIELD,
            &&OP_CALL,
            &&OP_PUSH,
            &&OP_POP,
            &&OP_RETURN,
            &&OP_RETURN_I,
            &&OP_NOP,
            &&OP_FFRAME,
            &&OP_INC,
            &&OP_DEC,
            &&OP_MOV_FNC,
            //start compare and jump group
            &&OP_JL,
            &&OP_JG,
            &&OP_JLE,
            &&OP_JGE,
            &&OP_JE,
            &&OP_JNE,
            // end compare and jump group
            // start immediate compare and jmp group
            &&OP_JL_I,
            &&OP_JG_I,
            &&OP_JLE_I,
            &&OP_JGE_I,
            &&OP_JE_I,
            &&OP_JNE_I,
            // end immediate compare and jmp group
            &&OP_IMPORT,
            &&OP_THROW,
            &&OP_SET_CATCH,
            &&OP_CLEAR_CATCH,
            &&OP_CREATE_THIS
    };
    fsize -= code_start;

    if (initial_state->instruction_pointer == NULL) {
        instruction_ptr = (z_instruction_t *) code;
    } else {
        instruction_ptr = (z_instruction_t *) (byte_stream + initial_state->instruction_pointer);
    }
    initial_state->return_code = 0;

    goto OP_FFRAME;

OP_SET_CATCH:
    {

        arraylist_t *catch_list = current_context->context_object.catches_list;
        if (!catch_list) {
            catch_list = arraylist_new(sizeof(int_t));
            initial_state->current_context = current_context;
            current_context->context_object.catches_list = catch_list;
        }
        arraylist_push(catch_list, (any_ptr_t) &instruction_ptr->r0);

        GOTO_NEXT;
    };
OP_CLEAR_CATCH:
    {
        arraylist_pop(current_context->context_object.catches_list);
        GOTO_NEXT;
    };
OP_THROW:
    {
        INIT_R0;
        initial_state->current_context = current_context;
        interpreter_throw_exception_from_reg(initial_state, r0);
        RETURN_IF_ERROR;
        GOTO_CATCH;
    };
OP_IMPORT:
    {
        INIT_R0;
        INIT_R1;
        char *import = strdup(data + instruction_ptr->r0);
        char *as = strdup(data + instruction_ptr->r1);
        map_insert(get_imports_table(initial_state), as, &import);
        GOTO_NEXT;
    };
OP_MOV_NUMBER:
    {
        INIT_R0;
        r0->number_val = (instruction_ptr->r1_float);
        r0->type = TYPE_NUMBER;
        GOTO_NEXT;
    };
OP_MOV_FNC:
    {
        INIT_R0;
        r0->val = (int_t) function_ref_new(instruction_ptr->r1, current_context, initial_state, instruction_ptr->r2);
        r0->type = TYPE_FUNCTION_REF;
        ADD_OBJECT_TO_GC_LIST(r0->val);
        GOTO_NEXT;
    };
OP_MOV_STR :
    {
        INIT_R0;
        char *str = data + instruction_ptr->r1;
        char *copied = (char *) z_alloc_or_gc(strlen(str) + 1);
        strcpy(copied, str);
        r0->val = (int_t) string_new(copied);
        r0->type = TYPE_STR;
        ADD_OBJECT_TO_GC_LIST(r0->val);
        GOTO_NEXT
    };
OP_MOV :
    {
        INIT_R0;
        INIT_R1;
        *r0 = *r1;
        GOTO_NEXT
    };
OP_INC:
    {
        INIT_R0;
        INIT_R1;
        r0->number_val++;
        *r1 = *r0;
        GOTO_NEXT;
    };
OP_DEC:
    {
        INIT_R0;
        INIT_R1;
        r0->number_val--;
        *r1 = *r0;
        GOTO_NEXT;
    };
OP_ADD :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        if (r0->type == TYPE_NUMBER) {
            if (r1->type == TYPE_NUMBER) {
                r2->number_val = ((r0->number_val) + (r1->number_val));
                r2->type = TYPE_NUMBER;
            } else {
                z_object_t *obj = (z_object_t *) r1->val;
                char *num_str = num_to_str(r0->number_val);
                char *str = strconcat(num_str, (const char *) obj->operations.to_string(obj));
                z_free(num_str);
                r2->val = (uint_t) string_new(str);
                r2->type = TYPE_STR;
                ADD_OBJECT_TO_GC_LIST(r2->val);
            }
        } else {
            char *ret_str = NULL;
            z_object_t *obj1 = (z_object_t *) r0->val;
            if (r1->type == TYPE_NUMBER) {
                char *num_str = num_to_str(r1->number_val);
                ret_str = strconcat((const char *) obj1->operations.to_string(obj1), num_str);
                z_free(num_str);
            } else {
                z_object_t *obj2 = (z_object_t *) r1->val;
                ret_str = strconcat((const char *) obj1->operations.to_string(obj1),
                                    (const char *) obj2->operations.to_string(obj2));
            }
            z_object_t *result = string_new(ret_str);
            r2->type = TYPE_STR;
            r2->val = (int_t) (result);
            ADD_OBJECT_TO_GC_LIST(r2->val);
        }

        GOTO_NEXT;
    };

OP_SUB :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = ((r0->number_val) - (r1->number_val));
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_DIV :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        if (r1->number_val == 0) {
            interpreter_throw_exception_from_str(initial_state, (char *) "divide by 0");
            RETURN_IF_ERROR;
            GOTO_CATCH;
        }
        r2->number_val = ((r0->number_val) / (r1->number_val));
        r2->type = TYPE_NUMBER;
        GOTO_NEXT;
    };
OP_MUL :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = ((r0->number_val) * (r1->number_val));
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_MOD :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        if (r0->type == TYPE_NUMBER) {
            r2->number_val = fmod(r0->number_val, r1->number_val);
            r2->type = TYPE_NUMBER;
        }
        GOTO_NEXT
    };
OP_CMP_EQUAL :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = (r0->number_val == r1->number_val);
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_CMP_LESS :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = ((r0->number_val) < (r1->number_val));
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_CMP_LESS_OR_EQUAL:
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = ((r0->number_val) <= (r1->number_val));
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_CMP_GREATER :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = ((r0->number_val) > (r1->number_val));
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_CMP_N_EQUAL :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = (r0->number_val != r1->number_val);
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_CMP_GREATER_OR_EQUAL :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = ((r0->number_val) >= (r1->number_val));
        r2->type = TYPE_NUMBER;
        GOTO_NEXT
    };
OP_JMP_TRUE :
    {
        INIT_R0;
        if (r0->val) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r1);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JMP_NOT_TRUE :
    {
        INIT_R0;
        if (!r0->val) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r1);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JMP :
    {
        instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r0);
        GOTO_CURRENT;
    };
OP_JL :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) < (r1->number_val))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JLE :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) <= (r1->number_val))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JG :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) > (r1->number_val))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JGE :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) >= (r1->number_val))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JE :
    {
        INIT_R0;
        INIT_R1;
        if (r0->number_val == r1->number_val) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JNE :
    {
        INIT_R0;
        INIT_R1;
        if (r0->number_val != r1->number_val) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JL_I :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) < (instruction_ptr->r1_float))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JLE_I :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) <= (instruction_ptr->r1_float))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JG_I :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) > (instruction_ptr->r1_float))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JGE_I :
    {
        INIT_R0;
        INIT_R1;
        if (((r0->number_val) >= (instruction_ptr->r1_float))) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JE_I :
    {
        INIT_R0;
        INIT_R1;
        if (r0->val == instruction_ptr->r1) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JNE_I :
    {
        INIT_R0;
        if (r0->val != instruction_ptr->r1) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_GET_FIELD:
    {
        INIT_R1;
        if (r1->type == TYPE_NUMBER) {
            field_name_to_get = num_to_str(r1->number_val);
        } else {
            field_name_to_get = ((z_object_t *) r1->val)->operations.to_string((void *) r1->val);
        }
        goto fieldNameSet;
    };
OP_GET_FIELD_IMMEDIATE :
    {
        field_name_to_get = data + instruction_ptr->r1;
    fieldNameSet:;
        uint_t lookup_object = instruction_ptr->r0;
        //first search native functions
        z_native_fnc_t *native_fnc_ptr_ptr = (z_native_fnc_t *) map_get(native_functions, field_name_to_get);
        if (native_fnc_ptr_ptr) {
            INIT_R2;
            r2->val = (uint_t) (*native_fnc_ptr_ptr);
            r2->type = TYPE_NATIVE_FUNC;
        } else if (lookup_object == 1) {
        SEARCH_THIS:
            INIT_R2;
            //search `this`
            z_object_t *context = current_context;
            while (context != NULL) {
                LOCK_SYMBOL_TABLE
                map_t *symbol_table = context->context_object.symbol_table;
                if (!symbol_table) {
                    //build symbol table
                    symbol_table = build_symbol_table(
                            byte_stream + context->context_object.symbols_address
                    );
                    context->context_object.symbol_table = symbol_table;
                }
                int_t *index_ptr = ((int_t *) map_get(symbol_table, field_name_to_get));
                UNLOCK_SYMBOL_TABLE
                if (index_ptr) {
                    int_t index = *index_ptr;
                    *r2 = *(((z_reg_t *) context->context_object.locals) + index);
                    //found!
                    GOTO_NEXT;
                }
                context = (z_object_t *) context->context_object.parent_context;
            }
            LOCK_SYNCHRONIZED_ACCESS
            //not found? maybe a synchronized variable?
            z_reg_t *prop = (z_reg_t *) map_get(initial_state->synchronized_variables, field_name_to_get);
            if (prop) {
                *r2 = *prop;
                UNLOCK_SYNCHRONIZED_ACCESS
                GOTO_NEXT;
            }
            UNLOCK_SYNCHRONIZED_ACCESS

            //not found? maybe a static variable?
            char *class_name = initial_state->class_name;
            z_type_info_t *type_info = (z_type_info_t *) map_get(known_types_map, class_name);
            if (type_info) {
                z_reg_t *prop = (z_reg_t *) map_get(type_info->static_variables, field_name_to_get);
                if (prop) {
                    *r2 = *prop;
                    GOTO_NEXT;
                }
            }
            //not found? maybe a class constructor?
            r2->val = (int_t) class_ref_new(field_name_to_get);
            r2->type = TYPE_CLASS_REF;
            ADD_OBJECT_TO_GC_LIST(r2->val);
        } else {
            //access r0 and search upon it
            INIT_R0;
            INIT_R2;
            if (r0->type != TYPE_NUMBER) {
                object_to_search_on = (z_object_t *) r0->val;
                if (r0->type == TYPE_CLASS_REF) {
                    //static method call
                    char *class_name = ((z_object_t *) r0->val)->class_ref_object.value;
                    z_type_info_t *type_info = object_manager_get_or_load_type_info(class_name,
                                                                                    get_imports_table(initial_state));
                    z_reg_t *prop = (z_reg_t *) map_get(type_info->static_variables, field_name_to_get);
                    if (prop) {
                        *r2 = *prop;
                    } else {
                        interpreter_throw_exception_from_str(initial_state, "cannot read property");
                        RETURN_IF_ERROR;
                        GOTO_CATCH;
                    }
                } else if (r0->type != TYPE_INSTANCE) {
                    z_reg_t *prop = (z_reg_t *) map_get(object_to_search_on->properties,
                                                        field_name_to_get);
                    if (prop) {
                        *r2 = *prop;
                    } else {
                        interpreter_throw_exception_from_str(initial_state, "cannot read property");
                        RETURN_IF_ERROR;
                        GOTO_CATCH;
                    }
                } else {
                    //get field from an object instance
                    object_to_search_on = (z_object_t *) r0->val;
                    z_interpreter_state_t *state = object_to_search_on->instance_object.saved_state;
                    z_reg_t *prop = interpreter_get_field_virtual(state, initial_state, field_name_to_get);
                    if (prop) {
                        *r2 = *prop;
                    } else {
                        interpreter_throw_exception_from_str(initial_state, "cannot read property on object");
                        RETURN_IF_ERROR;
                        GOTO_CATCH;
                    }
                }
            }
                /*the second local variable is this and the second is NULL. in both cases, goto SEARCH_THIS*/
            else if (r0->number_val == 0 || r0->number_val == 1) {
                goto SEARCH_THIS;
            } else {
                //TODO property access on number variables
            }
        }
        GOTO_NEXT;
    };
OP_SET_FIELD :
    {
        INIT_R1;
        if (r1->type == TYPE_NUMBER) {
            field_name_to_get = num_to_str(r1->number_val);
        } else {
            field_name_to_get = strdup(((z_object_t *) r1->val)->operations.to_string((void *) r1->val));
        }
        INIT_R0;
        INIT_R2;
        //set field of this...
        if (instruction_ptr->r0 == 1) {
            //iterate through scopes and set the value when you find
            z_object_t *context = current_context;
            while (context != NULL) {
                LOCK_SYMBOL_TABLE
                map_t *symbol_table = context->context_object.symbol_table;
                if (!symbol_table) {
                    //build symbol table
                    symbol_table = build_symbol_table(
                            byte_stream + context->context_object.symbols_address
                    );
                    context->context_object.symbol_table = symbol_table;
                }
                int_t *index_ptr = ((int_t *) map_get(symbol_table, field_name_to_get));
                UNLOCK_SYMBOL_TABLE
                if (index_ptr) {
                    int_t index = *index_ptr;
                    *(((z_reg_t *) context->context_object.locals) + index) = *r2;
                    //found!
                    GOTO_NEXT;
                }
                context = (z_object_t *) context->context_object.parent_context;
            }
            // TODO : create a synchronized objects table and make sure that the entry to set is actually there so that
            // tere is no such case like creating a synchronized variable accidentally.
            LOCK_SYNCHRONIZED_ACCESS
            // not found? maybe a synchronized variable?
            map_insert(initial_state->synchronized_variables, field_name_to_get, r2);
            UNLOCK_SYNCHRONIZED_ACCESS
            GOTO_NEXT;
        } else if (r0->type != TYPE_NUMBER) {
            if (r0->type != TYPE_INSTANCE) {
                if (r0->type == TYPE_CLASS_REF) {
                    char *class_name = ((z_object_t *) r0->val)->class_ref_object.value;
                    z_type_info_t *type_info;
                    //static has a special meaning for us
                    //it means that i am searching for a static variable but a static variable of mine not any other class
                    if (strcmp(class_name, "__static__") == 0) {
                        class_name = initial_state->class_name;
                        //since we are searching though our static variables, import table can be null
                        type_info = object_manager_get_or_load_type_info(class_name, NULL);
                    } else {
                        //give function to our current import table so that it can resolve what class to search upon
                        type_info = object_manager_get_or_load_type_info(class_name, get_imports_table(initial_state));
                    }
                    map_insert(type_info->static_variables, field_name_to_get, r2);
                } else {
                    object_to_search_on = (z_object_t *) r0->val;
                    map_insert(object_to_search_on->properties, field_name_to_get, r2);
                    // we changed a property and the cache is now garbage.
                    object_to_search_on->key_list_cache = NULL;
                }
            } else {
                // set field by using symbol table of this instance
                object_to_search_on = (z_object_t *) r0->val;
                z_interpreter_state_t *state = object_to_search_on->instance_object.saved_state;
                if (interpreter_set_field_virtual(state, initial_state, field_name_to_get, r2)) {
                    if (state->return_code) {
                        interpreter_throw_exception_from_str(initial_state, state->exception_details);
                        RETURN_IF_ERROR;
                        GOTO_CATCH;
                    }
                }
            }
        }
        GOTO_NEXT;
    };
OP_CALL :
    {
        INIT_R0;
        if (r0->type == TYPE_NATIVE_FUNC) {
            //call native function
            z_native_fnc_t native_fnc = (z_native_fnc_t) r0->val;
            INIT_R1;
            struct z_native_return_value ret = native_fnc(initial_state->stack_ptr, r1, object_to_search_on);
            initial_state->stack_ptr = ret.stack_ptr;
            if (ret.error_message) {
                interpreter_throw_exception_from_str(initial_state, ret.error_message);
                RETURN_IF_ERROR;
                GOTO_CATCH;
            }
        } else if (r0->type == TYPE_FUNCTION_REF) {
            z_object_t *function_ref = (z_object_t *) r0->val;
            int_t arguments_count = instruction_ptr->r2;
            z_interpreter_set_arguments_count(initial_state, arguments_count);
            // someone else's function...
            if (function_ref->function_ref_object.responsible_interpreter_state != initial_state) {
                int_t cloned_state = 0;
                z_interpreter_state_t *other_state = function_ref->function_ref_object.responsible_interpreter_state;
                // if the same instance runs again, it crashes the vm because of a concurrency problem
                // to solve this, create a clone of the state.
                LOCK_INTERPRETER_IS_RUNNING
                if (other_state->is_running) {
                    other_state = clone_state(other_state);
                    cloned_state = 1;
                    ADD_ROOT_TO_GC_LIST(other_state);
                }
                UNLOCK_INTERPRETER_IS_RUNNING
                z_object_t *called_fnc = context_new();
                called_fnc->context_object.parent_context = function_ref->function_ref_object.parent_context;
                called_fnc->context_object.return_context = NULL;
                called_fnc->context_object.return_address = NULL;
                called_fnc->context_object.requested_return_register_index = instruction_ptr->r1;
                other_state->instruction_pointer = (function_ref->function_ref_object.start_address);
                other_state->stack_ptr = initial_state->stack_ptr;
                other_state->stack_start = initial_state->stack_start;
                other_state->current_context = called_fnc;
                z_interpreter_run(other_state);
                if (cloned_state) {
                    REMOVE_ROOT_FROM_GC_ROOT_LIST(other_state);
                    // TODO: dispose other_state, this leads to a memory leak
                }
                if (other_state->return_code) {
                    //exception thrown
                    interpreter_throw_exception_from_str(initial_state, other_state->exception_details);
                    RETURN_IF_ERROR;
                    GOTO_CATCH;
                }
                INIT_R1;
                *r1 = other_state->return_value;
                instruction_ptr++;
                GOTO_CURRENT;
            } else {
                //our function
                z_object_t *called_fnc = context_new();
                //call async function
                if (function_ref->function_ref_object.is_async) {
                    pthread_t *thread = (pthread_t *) z_alloc_or_die(sizeof(pthread_t));
                    z_async_fnc_args_t *args = (z_async_fnc_args_t *) z_alloc_or_die(
                            sizeof(z_async_fnc_args_t));
                    args->initial_state = initial_state;
                    args->function_ref = function_ref;
                    z_reg_t *copied_stack = CREATE_STACK(stack_file_size);
                    memcpy(copied_stack, initial_state->stack_start, stack_file_size * sizeof(z_reg_t));
                    args->stack_start = copied_stack;
                    args->stack_ptr = &copied_stack[(initial_state->stack_ptr - initial_state->stack_start)];
                    z_thread_gc_safe_start_thread(thread, args);
                    GOTO_NEXT;
                } else {
                    called_fnc->context_object.parent_context = function_ref->function_ref_object.parent_context;
                    called_fnc->context_object.return_context = current_context;
                    called_fnc->context_object.return_address = instruction_ptr;
                    called_fnc->context_object.requested_return_register_index = instruction_ptr->r1;
                    instruction_ptr = (z_instruction_t *) (byte_stream +
                                                           function_ref->function_ref_object.start_address);
                    current_context = called_fnc;
                    initial_state->current_context = current_context;
                }
            }
            goto OP_FFRAME;
        } else if (r0->type == TYPE_CLASS_REF) {
            INIT_R1;
            z_type_info_t *type_info = object_manager_get_or_load_type_info(initial_state->class_name, NULL);
            object_new(((z_object_t *) r0->val)->class_ref_object.value, type_info->imports_table,
                       initial_state->stack_start, initial_state->stack_ptr, r1);
        } else {
            interpreter_throw_exception_from_str(initial_state, "callee is not a function");
            RETURN_IF_ERROR;
            GOTO_CATCH;
        }
        GOTO_NEXT;
    };
OP_PUSH:
    {
        INIT_R0;
        *(++initial_state->stack_ptr) = *r0;
        GOTO_NEXT
    };
OP_POP :
    {
        INIT_R0;
        *r0 = *(initial_state->stack_ptr--);
        GOTO_NEXT
    };
OP_RETURN :
    {
        z_reg_t *actual_return_reg = (+instruction_ptr->r0 + locals_ptr);
        instruction_ptr = current_context->context_object.return_address;
        uint_t return_reg = current_context->context_object.requested_return_register_index;
        initial_state->current_context = current_context;
        current_context = (z_object_t *) current_context->context_object.return_context;
        if (current_context == NULL) {
            initial_state->return_value = *actual_return_reg;
            initial_state->return_code = 0;
            goto end;
        }
        locals_ptr = (z_reg_t *) current_context->context_object.locals;
        z_reg_t *requested_return_reg = (return_reg + locals_ptr);
        *requested_return_reg = *actual_return_reg;
        GOTO_NEXT;
    };
OP_RETURN_I :
    {

    };
OP_NOP :
    {
        GOTO_NEXT;
    }
OP_FFRAME :
    {
        uint_t sizeof_locals = (instruction_ptr->r0);
        uint_t byte_size_locals = sizeof(z_reg_t) * (sizeof_locals + 1);
        locals_ptr = (z_reg_t *) z_alloc_or_die(byte_size_locals);
        memset(locals_ptr, 0, byte_size_locals);

        int_t arguments_count = initial_state->stack_ptr->val;
        initial_state->stack_ptr--;
        // place arguments
        // note that the first 2 locals are reserved.
        // the 3rd var is the function itself
        for (int_t i = 0; i < arguments_count; i++) {
            locals_ptr[i + 3] = *(initial_state->stack_ptr--);
        }
        // place this variable
        z_object_t *root_context = (z_object_t *) (initial_state->root_context);
        if (root_context->context_object.locals != NULL)
            locals_ptr[1] = ((z_reg_t *) root_context->context_object.locals)[1];

        current_context->context_object.locals = locals_ptr;
        current_context->context_object.locals_count = sizeof_locals + 1;
        current_context->context_object.symbols_address = instruction_ptr->r1;
        ADD_OBJECT_TO_GC_LIST(current_context);

        //calculate registers
        if (instruction_ptr->r2) {
            int_t end_of_function = instruction_ptr->r2;
            int_t start_of_function = ((uint_t) (instruction_ptr)) - (uint_t) byte_stream;
            int_t bytes_to_go = end_of_function - start_of_function;

            for (long i = 0; i < bytes_to_go; i += sizeof(z_instruction_t)) {
                z_instruction_t *instruction = (z_instruction_t *) (i + (uint_t) instruction_ptr);
                instruction->opcode = (uint_t) dispatch_table[(instruction)->opcode];
            }
            instruction_ptr->r2 = 0;
        }
        GOTO_NEXT;
    }
OP_CREATE_THIS :
    {
        // create 'this'
        z_object_t *self = (z_object_t *) z_alloc_or_die(sizeof(z_object_t));
        self->instance_object.saved_state = initial_state;
        self->instance_object.type_info = (z_type_info_t *) map_get(known_types_map, initial_state->class_name);
        self->operations = object_operations;
        self->type = TYPE_INSTANCE;
        locals_ptr[1].type = TYPE_INSTANCE;
        locals_ptr[1].val = (int_t) self;
        GOTO_NEXT;
    };
end:
    LOCK_INTERPRETER_IS_RUNNING
    initial_state->is_running = 0;
    UNLOCK_INTERPRETER_IS_RUNNING
    return initial_state;
}

void z_interpreter_set_arguments_count(z_interpreter_state_t *initial_state, int_t arguments_count) {
    z_reg_t args_count_reg;
    args_count_reg.type = TYPE_NUMBER;
    args_count_reg.val = arguments_count;

    initial_state->stack_ptr++;
    *initial_state->stack_ptr = args_count_reg;
}

z_interpreter_state_t *clone_state(z_interpreter_state_t *state) {
    z_interpreter_state_t *clone = interpreter_state_new(state->current_context, state->byte_stream, state->fsize,
                                                         state->class_name, state->stack_ptr, state->stack_start);
    return clone;
}

void goto_catch_block(const z_interpreter_state_t *initial_state,
                      const char *byte_stream,
                      z_object_t **current_context_ptr_ptr,
                      z_instruction_t **instruction_ptr_ptr,
                      z_reg_t **locals_ptr) {
    *instruction_ptr_ptr = (z_instruction_t *) (byte_stream + initial_state->instruction_pointer);
    *current_context_ptr_ptr = (z_object_t *) initial_state->current_context;
    *locals_ptr = (z_reg_t *) (*current_context_ptr_ptr)->context_object.locals;
}

/**
 * runs static constructor of a given compiled class.
 * @param bytes
 */
void interpreter_run_static_constructor(char *bytes, uint_t len, char *class_name) {
    int_t *static_block_ptr_ptr = (int_t *) (bytes + sizeof(int_t));
    int_t static_block_ptr = *static_block_ptr_ptr;
    z_object_t *context = context_new();
    z_interpreter_state_t *temp_state = interpreter_state_new(context, bytes, len, class_name, NULL,
                                                              NULL);
    z_interpreter_set_arguments_count(temp_state, 0);
    temp_state->instruction_pointer = static_block_ptr;
    z_interpreter_run(temp_state);
    ADD_ROOT_TO_GC_LIST(temp_state);
}

/**
 * get a field of an object.
 * @param objects_state saved state of the object.
 * @param objects_state saved state of our's.
 * @param field_name_to_get self explanatory.
 * @return register type.
 */
z_reg_t *interpreter_get_field_virtual(z_interpreter_state_t *objects_state, z_interpreter_state_t *our_state,
                                       char *field_name_to_get) {
    z_object_t *context = (z_object_t *) objects_state->root_context;
    while (context != NULL) {
        LOCK_SYMBOL_TABLE
        map_t *symbol_table = context->context_object.symbol_table;
        if (!symbol_table) {
            //build symbol table
            symbol_table = build_symbol_table(
                    objects_state->byte_stream + context->context_object.symbols_address
            );
            context->context_object.symbol_table = symbol_table;
        }
        int_t flags = 0;
        int_t *index_ptr = ((int_t *) map_get_flags(symbol_table, field_name_to_get, &flags));
        UNLOCK_SYMBOL_TABLE
        if (index_ptr) {
            if ((flags & MAP_FLAG_PRIVATE)) {
                if (strcmp(our_state->class_name, objects_state->class_name) != 0) {
                    return NULL;
                }
            }
            int_t index = *index_ptr;
            return (((z_reg_t *) context->context_object.locals) + index);
            //found!
        }
        context = (z_object_t *) context->context_object.parent_context;
    }
    return NULL;
}

/**
 * set a field of an object.
 * @param objects_state saved state of the object.
 * @param field_name_to_set self explanatory.
 * @param value value of the field.
 * @returns non zero if threw exception.
 */
int_t interpreter_set_field_virtual(z_interpreter_state_t *objects_state, z_interpreter_state_t *our_state,
                                    char *field_name_to_set, z_reg_t *value) {
    objects_state->return_code = 0;
    z_object_t *context = (z_object_t *) objects_state->root_context;
    LOCK_SYMBOL_TABLE
    map_t *symbol_table = context->context_object.symbol_table;
    if (!symbol_table) {
        //build symbol table
        symbol_table = build_symbol_table(
                objects_state->byte_stream + context->context_object.symbols_address
        );
        context->context_object.symbol_table = symbol_table;
    }
    int_t flags = 0;
    int_t *index_ptr = ((int_t *) map_get_flags(symbol_table, field_name_to_set, &flags));
    UNLOCK_SYMBOL_TABLE
    if (index_ptr) {
        if ((flags & MAP_FLAG_PRIVATE)) {
            if (strcmp(our_state->class_name, objects_state->class_name) != 0) {
                interpreter_throw_exception_from_str(objects_state, "cannot set private property of object");
            }
        }
        int_t index = *index_ptr;
        *(((z_reg_t *) context->context_object.locals) + index) = *value;
    } else {
        interpreter_throw_exception_from_str(objects_state, "no such property found on object");
        return 1;
    }
    return 0;
}
/**
 * get current imports table.
 * @param initial_state interpreter state.
 * @return map_t.
 */
Z_INLINE map_t *get_imports_table(z_interpreter_state_t *initial_state) {
    char *class_name = initial_state->class_name;
    z_type_info_t *type_info = object_manager_get_or_load_type_info(class_name, NULL);
    return type_info->imports_table;
}

void interpreter_throw_exception_from_str(z_interpreter_state_t *current_state, char *message) {
    //TODO: add function lineage and line numbers
    z_object_t *context = (z_object_t *) current_state->current_context;
    while (context != NULL) {
        arraylist_t *catch_points_list = context->context_object.catches_list;
        if (catch_points_list && catch_points_list->size > 0) {
            int_t catch_ptr = *(int_t *) arraylist_top(catch_points_list);
            current_state->instruction_pointer = catch_ptr;
            z_reg_t *exception_info = (z_reg_t *) z_alloc_or_die(sizeof(z_reg_t));
            exception_info->type = TYPE_STR;
            z_object_t *exception_info_str = string_new(message);
            exception_info->val = (uint_t) exception_info_str;
            *++current_state->stack_ptr = *exception_info;
            return;
        } else {
            context = (z_object_t *) context->context_object.return_context;
            current_state->current_context = context;
        }
    }
    char *detailed_message = NULL;
    char *class_name = current_state->class_name;
    int_t length = strlen(message) + strlen(class_name) + 100;
    detailed_message = (char *) z_alloc_or_die((size_t) length);
    snprintf(detailed_message, length, "%s \nclass:%s", message, class_name);
    current_state->return_code = 1;
    current_state->exception_details = detailed_message;
}

void interpreter_throw_exception_from_reg(z_interpreter_state_t *current_state, z_reg_t *object_thrown) {
    char *message = NULL;
    if (object_thrown->type == TYPE_NUMBER) {
        message = num_to_str(object_thrown->number_val);
    } else {
        message = ((z_object_t *) object_thrown->val)->operations.to_string((void *) object_thrown->val);
    }
    interpreter_throw_exception_from_str(current_state, message);
}

z_interpreter_state_t *
interpreter_state_new(void *current_context, char *bytes, int_t len, char *class_name, z_reg_t *stack_ptr,
                      z_reg_t *stack_start) {
    if (!stack_start) {
        stack_start = CREATE_STACK(stack_file_size);
        stack_ptr = stack_start;
    }
    if (current_context == NULL) {
        current_context = context_new();
    }

    z_interpreter_state_t *initial_state = (z_interpreter_state_t *) z_alloc_or_die(sizeof(z_interpreter_state_t));
    initial_state->fsize = len;
    initial_state->byte_stream = bytes;
    initial_state->current_context = current_context;
    initial_state->instruction_pointer = NULL;
    initial_state->class_name = class_name;
    initial_state->stack_ptr = stack_ptr;
    initial_state->return_code = 0;
    initial_state->exception_details = NULL;
    initial_state->root_context = current_context;
    initial_state->stack_start = stack_start;
    initial_state->synchronized_variables = map_new(sizeof(z_reg_t));
    initial_state->removed_as_a_root = 0;
    initial_state->is_running = 0;
    return initial_state;
}
