//
// Created by onur on 24.05.2018.
//

#ifndef ZEROSCRIPT_INSTRUCTION_H
#define ZEROSCRIPT_INSTRUCTION_H

#include <stdint-gcc.h>

//-----------instructions
//mov immediate number at r1 into r0
#define MOV_NUMBER 0
//mov immediate string at r1 into r0
#define MOV_STR 1
//mov contents of r1 to r0
#define MOV 2
//r2 = r1 + r2
#define ADD 3
//r2 = r1 - r2
#define SUB 4
//r2 = r1 / r2
#define DIV 5
//r2 = r1 * r2
#define MUL 6
//r2 = r1 % r2
#define MOD 7
//r2 = r1 == r2
#define CMP_EQUAL 8
//r2 = r0 < r1
#define CMP_LESS 9
//r2 = r0 <= r1
#define CMP_LESS_OR_EQUAL 10
//r2 = r0 > r1
#define CMP_GREATER 11
//r2 = r0 == r1
#define CMP_N_EQUAL 12
//r2 = r0 != r1
#define CMP_GREATER_OR_EQUAL 13
//jmp to r1 if r0==1
#define JMP_TRUE 14
//jmp to r1 if r0!=1
#define JMP_NOT_TRUE 15
//jmp to r0
#define JMP 16
//get r1 field of r0 into r2
#define GET_FIELD 17

#define GET_FIELD_IMMEDIATE 18
//set r1 field of r0 into r2
#define SET_FIELD 19
//call function in label r0 into r1
#define CALL 20
//push r1
#define PUSH 21
//pop into r1
#define POP 22

#define RETURN 23

#define RETURN_I 24

#define NOP 25

#define FFRAME 26

#define INC 27

#define DEC 28

#define MOV_FNC 29

//****super instructions****//
//jump to r2 if r0 < r1
#define JMP_LESS 30

//jump to r2 if r0 > r1
#define JMP_GREATER 31

//jump to r2 if r0 > r1
#define JMP_LESS_OR_EQUAL 32

//jump to r2 if r0 > r1
#define JMP_GREATER_OR_EQUAL 33

//jump to r2 if r0 == r1
#define JMP_EQUAL 34

//jump to r2 if r0 != r1
#define JMP_N_EQUAL 35

//jump to r2 if r0 < i1
#define JMP_LESS_I 36

//jump to r2 if r0 > i1
#define JMP_GREATER_I 37

//jump to r2 if r0 > i1
#define JMP_LESS_OR_EQUAL_I 38

//jump to r2 if r0 > i1
#define JMP_GREATER_OR_EQUAL_I 39

//jump to r2 if r0 == i1
#define JMP_EQUAL_I 40

//jump to r2 if r0 != i1
#define JMP_N_EQUAL_I 41

//import table builder
#define IMPORT_CLS 42


//throw r0
#define THROW_EXCEPTION 43

//set catch point at r0
#define SET_CATCH 44

//clear topmost catch point
#define CLEAR_CATCH 45

#define CREATE_THIS 46

//****pseudo instructions****//
#define COMMENT 47

#define LABEL 48


typedef struct z_instruction_t {
    union {
        struct {
            uint_t opcode;
        };
        struct {
            uhalf_int_t short_opcode;
            uhalf_int_t line_number;
        };
    };
    uint_t r0;  //index of a local
    union {
        uint_t r1;  //index of a local
        FLOAT r1_float;
    };
    uint_t r2;  //index of a local
} z_instruction_t;

z_instruction_t *instruction_new(uint_t opcode, uint_t r0, uint_t r1, uint_t r2) {
    z_instruction_t *self = (z_instruction_t *) (z_alloc_or_die(sizeof(z_instruction_t)));
    self->opcode = opcode;
    self->r0 = r0;
    self->r1 = r1;
    self->r2 = r2;
    return self;
}

static const char *name_opcode(uint_t opcode);

void instruction_print(z_instruction_t instruction) {
    if (instruction.opcode == CREATE_THIS) {
        printf("\tcreate_this");
    } else if (instruction.opcode == SET_CATCH) {
        printf("\t%-5s %-5s", name_opcode(SET_CATCH), (char *) (instruction.r0));
    } else if (instruction.opcode == CLEAR_CATCH) {
        printf("\t%-5s", name_opcode(CLEAR_CATCH));
    } else if (instruction.opcode >= JMP_LESS && instruction.opcode <= JMP_N_EQUAL) {
        printf("\t%-5s $%-5d $%-5d %-5s ", name_opcode(instruction.opcode), (int) instruction.r0, (int) instruction.r1,
               (char *) instruction.r2);
    }  else if (instruction.opcode >= JMP_LESS_I && instruction.opcode <= JMP_N_EQUAL_I) {
        printf("\t%-5s $%-5d %-5s %-5s ", name_opcode(instruction.opcode), (int) instruction.r0, (char*) instruction.r1,
               (char *) instruction.r2);
    } else if (instruction.opcode == THROW_EXCEPTION) {
        printf("\t%s $%-5d", name_opcode(instruction.opcode), (int) (instruction.r0));
    } else if (instruction.opcode == IMPORT_CLS) {
        printf("\t%s %-5s %-5s:", name_opcode(instruction.opcode), (char *) instruction.r0, (char *) instruction.r1);
    } else if (instruction.opcode == LABEL) {
        printf("%s %-5s:", name_opcode(instruction.opcode), (char *) instruction.r0);
    } else if (instruction.opcode == COMMENT) {
        printf("\t#%s %-5s", name_opcode(instruction.opcode), (char *) instruction.r0);
    } else if (instruction.opcode == FFRAME) {
        printf("\t%-5s %-5d", name_opcode(instruction.opcode), (int) instruction.r0);
    } else if (instruction.opcode == CALL) {
        printf("\t%-5s $%-5d $%-5d %-5d", name_opcode(instruction.opcode), (int) instruction.r0, (int) instruction.r1, (int) instruction.r2);
    } else if (instruction.opcode == GET_FIELD || instruction.opcode == SET_FIELD) {
        printf("\t%-5s $%-5d $%-5d $%-5d", name_opcode(instruction.opcode), (int) instruction.r0,
               (int) instruction.r1, (int) instruction.r2);
    } else if (instruction.opcode == GET_FIELD_IMMEDIATE) {
        printf("\t%-5s $%-5d %-5s $%-5d", name_opcode(instruction.opcode), (int) instruction.r0,
               (char *) instruction.r1, (int) instruction.r2);
    } else if (instruction.opcode == RETURN) {
        printf("\t%-5s $%-5d ", name_opcode(instruction.opcode), (int) instruction.r0);
    } else if (instruction.opcode == RETURN_I) {
        printf("\t%-5s %-5d ", name_opcode(instruction.opcode), (int) instruction.r0);
    } else if (instruction.opcode == JMP_TRUE || instruction.opcode == JMP_NOT_TRUE) {
        printf("\t%-5s $%-5d %-5s ", name_opcode(instruction.opcode), (int) instruction.r0, (char *) instruction.r1);
    } else if (instruction.opcode == JMP) {
        printf("\t%-5s %-5s ", name_opcode(instruction.opcode), (char *) instruction.r0);
    } else if (instruction.opcode == PUSH || instruction.opcode == POP) {
        printf("\t%-5s $%-5d ", name_opcode(instruction.opcode), (int) instruction.r0);
    } else if (instruction.opcode == MOV_STR || instruction.opcode == MOV_NUMBER || instruction.opcode == MOV_FNC) {
        printf("\t%-5s $%-5d %-5s ", name_opcode(instruction.opcode), (int) instruction.r0, (char *) instruction.r1);
    } else if (instruction.opcode == MOV) {
        printf("\t%-5s $%-5d $%-5d ", name_opcode(instruction.opcode), (int) instruction.r0, (int) instruction.r1);
    } else {
        printf("\t%-5s $%-5d $%-5d $%-5d ", name_opcode(instruction.opcode), (int) instruction.r0, (int) instruction.r1,
               (int) instruction.r2);
    }
}

static const char *name_opcode(uint_t opcode) {
    switch (opcode) {
        case SET_CATCH:
            return "set_catch";
        case CLEAR_CATCH:
            return "clear_catch";
        case THROW_EXCEPTION:
            return "throw";
        case JMP_GREATER_OR_EQUAL:
            return "jge";
        case JMP_GREATER:
            return "jg";
        case JMP_LESS_OR_EQUAL:
            return "jle";
        case JMP_EQUAL:
            return "je";
        case JMP_N_EQUAL:
            return "jne";
        case JMP_LESS:
            return "jl";
        case JMP_GREATER_OR_EQUAL_I:
            return "jge_i";
        case JMP_GREATER_I:
            return "jg_i";
        case JMP_LESS_OR_EQUAL_I:
            return "jle_i";
        case JMP_EQUAL_I:
            return "je_i";
        case JMP_N_EQUAL_I:
            return "jne_i";
        case JMP_LESS_I:
            return "jl_i";
        case DEC:
            return "dec";
        case INC:
            return "inc";
        case MOV_FNC:
            return "mov_f";
        case LABEL:
            return "";
        case COMMENT:
            return "";
        case FFRAME:
            return "fframe";
        case MOV_NUMBER:
            return "mov_n";
        case MOV_STR:
            return "mov_s";
        case MOV:
            return "mov";
        case ADD:
            return "add";
        case SUB:
            return "sub";
        case MUL:
            return "mul";
        case MOD:
            return "mod";
        case DIV:
            return "div";
        case CMP_EQUAL:
            return "ce";
        case CMP_N_EQUAL:
            return "cne";
        case CMP_GREATER:
            return "cg";
        case CMP_GREATER_OR_EQUAL:
            return "cge";
        case CMP_LESS:
            return "cl";
        case CMP_LESS_OR_EQUAL:
            return "cle";
        case JMP:
            return "jmp";
        case JMP_NOT_TRUE:
            return "jnt";
        case JMP_TRUE:
            return "jt";
        case GET_FIELD:
            return "get_f";
        case GET_FIELD_IMMEDIATE:
            return "get_fi";
        case SET_FIELD:
            return "set_f";
        case CALL:
            return "call";
        case RETURN:
            return "ret";
        case RETURN_I:
            return "ret_i";
        case PUSH:
            return "push";
        case POP:
            return "pop";
        case NOP:
            return "nop";
        case IMPORT_CLS:
            return "import";
        default:
            return "UNKNOWN";
    }
}

#endif //ZEROSCRIPT_INSTRUCTION_H
