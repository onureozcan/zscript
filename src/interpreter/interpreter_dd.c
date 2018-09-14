//
// Created by onur on 06.06.2018.
//

#define TYPE_NUMBER 0
#define TYPE_STR 1
#define TYPE_FUNCTION_REF 4
#define TYPE_OBJ 16
#define TYPE_NATIVE_FUNC 32
#define TYPE_INSTANCE 64
#define TYPE_CONSTRUCTOR_REF 128

//#define PRE_CALCULATE_JUMP_POINTERS

#define INIT_R0 z_reg_t* r0 = locals_ptr + instruction_ptr->r0
#define INIT_R1 z_reg_t* r1 = locals_ptr + instruction_ptr->r1
#define INIT_R2 z_reg_t* r2 = locals_ptr + instruction_ptr->r2

#ifndef PRE_CALCULATE_JUMP_POINTERS

#define GOTO_NEXT goto *dispatch_table[(++instruction_ptr)->opcode];
#define GOTO_CURRENT goto *dispatch_table[(instruction_ptr)->opcode];

#else

#define GOTO_NEXT goto *(++instruction_ptr)->opcode;
#define GOTO_CURRENT goto *(instruction_ptr)->opcode;

#endif

#ifdef FLOAT_SUPPORT
#else
#endif


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

char *num_to_str(FLOAT val);

Z_INLINE static char *strconcat(const char *s1, const char *s2);


typedef struct z_interpreter_state_t {
    void *current_context;
    char *byte_stream;
    int_t fsize;
    z_instruction_t *instruction_pointer;
    z_reg_t return_value;
} z_interpreter_state_t;

z_interpreter_state_t *z_interpreter_run(z_interpreter_state_t *initial_state);

/**
 * get a field of an object.
 * @param saved_state saved state of the object.
 * @param field_name_to_get self explanatory.
 * @return register type.
 */
z_reg_t *interpreter_get_field_virtual(z_interpreter_state_t *saved_state, char *field_name_to_get);

/**
 * set a field of an object.
 * @param saved_state saved state of the object.
 * @param field_name_to_set self explanatory.
 * @param value value of the field.
 */
void interpreter_set_field_virtual(z_interpreter_state_t *saved_state, char *field_name_to_set, z_reg_t *value);

#include "object.h"
#include "native_functions.c"
#include "object.c";

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

Z_INLINE static char *strconcat(const char *s1, const char *s2) {
    uint_t l1 = strlen(s1);
    uint_t l2 = strlen(s2) + 1;
    char *buff = (char *) z_alloc_or_die((size_t) (l1 + l2));
    memcpy(buff, s1, l1);
    memcpy(buff + l1, s2, l2);
    return buff;
}

#define stack_file_size 1000
z_reg_t stack_file[stack_file_size];

z_reg_t *stack_ptr = stack_file;

z_interpreter_state_t *z_interpreter_run(z_interpreter_state_t *initial_state) {

    char *byte_stream = initial_state->byte_stream;
    int_t fsize = initial_state->fsize;

    z_object_t *current_context = (z_object_t *) initial_state->current_context;

    if (initial_state->current_context == NULL) {
        current_context = context_new();
        current_context->context_object.parent_context = NULL;
        current_context->context_object.return_context = NULL;
    }

    if (!native_functions) {
        z_native_funcions_init();
    }

    char *field_name_to_get;

    z_object_t *object_to_search_on = NULL;

    current_context->ref_count++;

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
            &&OP_HLT,
            &&OP_NOP,
            &&OP_FFRAME,
            &&OP_INC,
            &&OP_DEC,
            &&OP_MOV_FNC,
            &&OP_JL,
            &&OP_JG,
            &&OP_JLE,
            &&OP_JGE,
            &&OP_JE,
            &&OP_JNE,
    };
    fsize -= code_start;
    //initialize jump properties
#ifdef PRE_CALCULATE_JUMP_POINTERS
    for (long i = 0; i < fsize; i += sizeof(z_instruction_t)) {
        instruction_ptr = (z_instruction_t *) (code + i);
        //printf("%s %d, %d, %d \n", name_opcode(instruction_ptr->opcode), instruction_ptr->r0, instruction_ptr->r1, instruction_ptr->r2);
        instruction_ptr->opcode = (uint_t) dispatch_table[(instruction_ptr)->opcode];
    }
#endif
    //exit(0);
    //reset current instruction
    if (initial_state->instruction_pointer == NULL) {
        instruction_ptr = (z_instruction_t *) code;
    } else {
        instruction_ptr = initial_state->instruction_pointer;
    }
    GOTO_CURRENT;
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
        r0->val = (int_t) function_ref_new(instruction_ptr->r1, current_context, initial_state);
        r0->type = TYPE_FUNCTION_REF;
        GOTO_NEXT;
    };
OP_MOV_STR :
    {
        INIT_R0;
        char *str = data + instruction_ptr->r1;
        r0->val = (int_t) string_new(str);
        r0->type = TYPE_STR;
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
        if (r0->type == TYPE_NUMBER) {
            r1->number_val = (r0->number_val) + 1;
            r1->type = TYPE_NUMBER;
        } else {
            //TODO:operator overloading
        }

        GOTO_NEXT;
    };
OP_DEC:
    {
        INIT_R0;
        INIT_R1;
        if (r0->type == TYPE_NUMBER) {
            r1->number_val = (r0->number_val) - 1;
            r1->type = TYPE_NUMBER;
        } else {
            //TODO:operator overloading
        }
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
                char *str = strconcat(num_to_str(r0->number_val), (const char *) obj->operations.to_string(obj));
                r2->val = (uint_t) string_new(str);
                r2->type = TYPE_STR;
            }
        } else {
            char *ret_str = NULL;
            z_object_t *obj1 = (z_object_t *) r0->val;
            if (r1->type == TYPE_NUMBER) {
                ret_str = strconcat((const char *) obj1->operations.to_string(obj1), num_to_str(r1->number_val));
            } else {
                z_object_t *obj2 = (z_object_t *) r1->val;
                ret_str = strconcat((const char *) obj1->operations.to_string(obj1),
                                    (const char *) obj2->operations.to_string(obj2));
            }
            z_object_t *result = string_new(ret_str);
            r2->type = TYPE_STR;
            r2->val = (int_t) (result);
        }

        GOTO_NEXT;
    };

OP_SUB :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        if (r0->type == TYPE_NUMBER) {
            r2->number_val = ((r0->number_val) - (r1->number_val));
            r2->type = TYPE_NUMBER;
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT
    };
OP_DIV :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        if (r0->type == TYPE_NUMBER) {
            r2->number_val = ((r0->number_val) / (r1->number_val));
            r2->type = TYPE_NUMBER;
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT;
    };
OP_MUL :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        if (r0->type == TYPE_NUMBER) {
            r2->number_val = ((r0->number_val) * (r1->number_val));
            r2->type = TYPE_NUMBER;
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT
    };
OP_MOD :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        if (r0->type == TYPE_NUMBER) {
            r2->number_val = (int_t) r0->number_val % (int_t) r1->number_val;
            r2->type = TYPE_NUMBER;
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT
    };
OP_CMP_EQUAL :
    {
        INIT_R0;
        INIT_R1;
        INIT_R2;
        r2->number_val = (r0->val == r1->val);
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
        if (r0->type == TYPE_NUMBER) {
            if (((r0->number_val) < (r1->number_val))) {
                instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
                GOTO_CURRENT;
            }
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT
    };
OP_JLE :
    {
        INIT_R0;
        INIT_R1;
        if (r0->type == TYPE_NUMBER) {
            if (((r0->number_val) <= (r1->number_val))) {
                instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
                GOTO_CURRENT;
            }
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT
    };
OP_JG :
    {
        INIT_R0;
        INIT_R1;
        if (r0->type == TYPE_NUMBER) {
            if (((r0->number_val) > (r1->number_val))) {
                instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
                GOTO_CURRENT;
            }
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT
    };
OP_JGE :
    {
        INIT_R0;
        INIT_R1;
        if (r0->type == TYPE_NUMBER) {
            if (((r0->number_val) >= (r1->number_val))) {
                instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
                GOTO_CURRENT;
            }
        } else {
            //TODO:operator overloading
        }
        GOTO_NEXT
    };
OP_JE :
    {
        INIT_R0;
        INIT_R1;
        if (r0->val == r1->val) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_JNE :
    {
        INIT_R0;
        INIT_R1;
        if (r0->val != r1->val) {
            instruction_ptr = (z_instruction_t *) (byte_stream + instruction_ptr->r2);
            GOTO_CURRENT;
        }
        GOTO_NEXT
    };
OP_GET_FIELD:
    {
        INIT_R1;
        if (r1->type == TYPE_NUMBER) {
            z_object_t *temp = string_new(num_to_str(r1->number_val));
            field_name_to_get = temp->operations.to_string(temp);
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
        native_fnc_t *native_fnc = (native_fnc_t *) map_get(native_functions, field_name_to_get);
        if (native_fnc) {
            INIT_R2;
            r2->val = (uint_t) native_fnc;
            r2->type = TYPE_NATIVE_FUNC;
        } else if (!lookup_object) {
            INIT_R2;
            //search `this`
            z_object_t *context = current_context;
            while (context != NULL) {
                map_t *symbol_table = context->context_object.symbol_table;
                if (!symbol_table) {
                    //build symbol table
                    symbol_table = build_symbol_table(
                            byte_stream + context->context_object.symbols_address
                    );
                    context->context_object.symbol_table = symbol_table;
                }
                int_t *index_ptr = ((int_t *) map_get(symbol_table, field_name_to_get));
                if (index_ptr) {
                    int_t index = *index_ptr;
                    *r2 = *(((z_reg_t *) context->context_object.locals) + index);
                    //found!
                    GOTO_NEXT;
                }
                context = (z_object_t *) context->context_object.parent_context;
            }
            //not found, maybe a class constructor?
            r2->type = TYPE_CONSTRUCTOR_REF;
            r2->val = (int_t) field_name_to_get;
        } else {
            //access r0 and search upon it
            INIT_R0;
            INIT_R2;
            if (r0->type != TYPE_NUMBER) {
                object_to_search_on = (z_object_t *) r0->val;
                if (r0->type != TYPE_INSTANCE) {
                    z_reg_t *prop = (z_reg_t *) map_get(object_to_search_on->properties,
                                                        field_name_to_get);
                    if (prop) {
                        *r2 = *prop;
                    } else {
                        error_and_exit("cannot read property");
                    }
                } else {
                    //get field from an object instance
                    object_to_search_on = (z_object_t *) r0->val;
                    z_interpreter_state_t *state = object_to_search_on->ordinary_object.saved_state;
                    z_reg_t *prop = interpreter_get_field_virtual(state, field_name_to_get);
                    if (prop) {
                        *r2 = *prop;
                    } else {
                        error_and_exit("cannot read property");
                    }
                }
            }
        }
        GOTO_NEXT;
    };
OP_SET_FIELD :
    {
        INIT_R1;
        if (r1->type == TYPE_NUMBER) {
            z_object_t *temp = string_new(num_to_str(r1->number_val));
            field_name_to_get = temp->operations.to_string(temp);
        } else {
            field_name_to_get = ((z_object_t *) r1->val)->operations.to_string((void *) r1->val);
        }
        INIT_R0;
        INIT_R2;
        //set field of this...
        if (instruction_ptr->r0 == 1) {
            //iterate through scopes and set the value when you find
            z_object_t *context = current_context;
            while (context != NULL) {
                map_t *symbol_table = context->context_object.symbol_table;
                if (!symbol_table) {
                    //build symbol table
                    symbol_table = build_symbol_table(
                            byte_stream + context->context_object.symbols_address
                    );
                    context->context_object.symbol_table = symbol_table;
                }
                int_t *index_ptr = ((int_t *) map_get(symbol_table, field_name_to_get));
                if (index_ptr) {
                    int_t index = *index_ptr;
                    *(((z_reg_t *) context->context_object.locals) + index) = *r2;
                    //found!
                    GOTO_NEXT;
                }
                context = (z_object_t *) context->context_object.parent_context;
            }
            error_and_exit("no such variable found to set");
        } else if (r0->type != TYPE_NUMBER) {
            if (r0->type != TYPE_INSTANCE) {
                object_to_search_on = (z_object_t *) r0->val;
                map_insert(object_to_search_on->properties, field_name_to_get, r2);
                //we changed a property and the cache is now garbage.
                object_to_search_on->key_list_cache = NULL;
            } else {
                //set field by using symbol table of this instance
                object_to_search_on = (z_object_t *) r0->val;
                z_interpreter_state_t *state = object_to_search_on->ordinary_object.saved_state;
                interpreter_set_field_virtual(state, field_name_to_get, r2);
            }
        }
        GOTO_NEXT;
    };
OP_CALL :
    {
        INIT_R0;
        if (r0->type == TYPE_NATIVE_FUNC) {
            //call native function
            native_fnc_t *native_fnc = (native_fnc_t *) r0->val;
            INIT_R1;
            stack_ptr = native_fnc->fnc(stack_ptr, r1, object_to_search_on);
        } else if (r0->type == TYPE_FUNCTION_REF) {
            z_object_t *function_ref = (z_object_t *) r0->val;
            //someone else's function...
            if (function_ref->function_ref_object.responsible_interpreter_state != initial_state) {
                z_interpreter_state_t *other_state = function_ref->function_ref_object.responsible_interpreter_state;
                z_object_t *called_fnc = context_new();
                called_fnc->context_object.parent_context = function_ref->function_ref_object.parent_context;
                called_fnc->context_object.return_context = NULL;
                called_fnc->context_object.return_address = NULL;
                called_fnc->context_object.requested_return_register_index = instruction_ptr->r1;
                other_state->instruction_pointer = (z_instruction_t *) (other_state->byte_stream +
                                                                        function_ref->function_ref_object.start_address);
                other_state->current_context = called_fnc;
                z_interpreter_run(other_state);
                INIT_R1;
                *r1 = other_state->return_value;
                instruction_ptr++;
            } else {
                //our function
                z_object_t *called_fnc = context_new();
                called_fnc->context_object.parent_context = function_ref->function_ref_object.parent_context;
                called_fnc->context_object.return_context = current_context;
                called_fnc->context_object.return_address = instruction_ptr;
                called_fnc->context_object.requested_return_register_index = instruction_ptr->r1;
                instruction_ptr = (z_instruction_t *) (byte_stream + function_ref->function_ref_object.start_address);
                current_context->ref_count++;
                current_context = called_fnc;
            }
            GOTO_CURRENT;
        } else if (r0->type == TYPE_CONSTRUCTOR_REF) {
            INIT_R1;
            r1->type = TYPE_INSTANCE;
            r1->val = (int_t) object_new((char *) r0->val);
        } else {
            error_and_exit("callee is not a function");
        }
        GOTO_NEXT;
    };
OP_PUSH:
    {
        INIT_R0;
        *(++stack_ptr) = *r0;
        GOTO_NEXT
    };
OP_POP :
    {
        INIT_R0;
        *r0 = *(stack_ptr--);
        GOTO_NEXT
    };
OP_RETURN :
    {
        z_reg_t *actual_return_reg = (+instruction_ptr->r0 + locals_ptr);
        instruction_ptr = current_context->context_object.return_address;
        uint_t return_reg = current_context->context_object.requested_return_register_index;
        current_context->ref_count--;
        initial_state->current_context = current_context;
        current_context = (z_object_t *) current_context->context_object.return_context;
        initial_state->return_value = *actual_return_reg;
        if (current_context == NULL) goto end;
        current_context->ref_count--;
        locals_ptr = (z_reg_t *) current_context->context_object.locals;
        z_reg_t *requested_return_reg = (return_reg + locals_ptr);
        *requested_return_reg = *actual_return_reg;
        GOTO_NEXT;
    };
OP_HLT :
    {
        goto end;
    }
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
        current_context->context_object.locals = locals_ptr;
        current_context->context_object.symbols_address = instruction_ptr->r1;
        GOTO_NEXT;
    }
end:
    return initial_state;
}

/**
 * get a field of an object.
 * @param saved_state saved state of the object.
 * @param field_name_to_get self explanatory.
 * @return register type.
 */
z_reg_t *interpreter_get_field_virtual(z_interpreter_state_t *saved_state, char *field_name_to_get) {
    z_object_t *context = (z_object_t *) saved_state->current_context;
    while (context != NULL) {
        map_t *symbol_table = context->context_object.symbol_table;
        if (!symbol_table) {
            //build symbol table
            symbol_table = build_symbol_table(
                    saved_state->byte_stream + context->context_object.symbols_address
            );
            context->context_object.symbol_table = symbol_table;
        }
        int_t *index_ptr = ((int_t *) map_get(symbol_table, field_name_to_get));
        if (index_ptr) {
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
 * @param saved_state saved state of the object.
 * @param field_name_to_set self explanatory.
 * @param value value of the field.
 */
void interpreter_set_field_virtual(z_interpreter_state_t *saved_state, char *field_name_to_set, z_reg_t *value) {
    z_object_t *context = (z_object_t *) saved_state->current_context;
    map_t *symbol_table = context->context_object.symbol_table;
    if (!symbol_table) {
        //build symbol table
        symbol_table = build_symbol_table(
                saved_state->byte_stream + context->context_object.symbols_address
        );
        context->context_object.symbol_table = symbol_table;
    }
    int_t *index_ptr = ((int_t *) map_get(symbol_table, field_name_to_set));
    if (index_ptr) {
        int_t index = *index_ptr;
        *(((z_reg_t *) context->context_object.locals) + index) = *value;
    } else {
        error_and_exit("no such property found ond object");
    }
}


