//
// Created by onur on 24.05.2018.
//

#ifndef ZEROSCRIPT_PROGRAM_H
#define ZEROSCRIPT_PROGRAM_H

#include "instruction.h"

typedef struct z_program_t {
    arraylist_t *instructions;
} z_program_t;

#include "Assembler.cpp"

uint_t z_program_add_instruction(z_program_t *program, uint_t opcode, uint_t r0, uint_t r1,
                                 uint_t r2);

z_instruction_t *z_program_get_latest_instruction(z_program_t *program) {
    uint_t i = program->instructions->size - 1;
    z_instruction_t *latest = static_cast<z_instruction_t *>(arraylist_get(program->instructions, i));
    while (latest->opcode == COMMENT && i > 0) {
        latest = static_cast<z_instruction_t *>(arraylist_get(program->instructions, i--));
    }
    return latest;
}

z_program_t *z_program_new() {
    z_program_t *self = (z_program_t *) (z_alloc_or_die(sizeof(z_program_t)));
    self->instructions = arraylist_new(sizeof(z_instruction_t));
    return self;
};

void z_program_add_label(z_program_t *program, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *buffer = (char *) (malloc(100));
    vsnprintf(buffer, 99, format, args);
    va_end(args);
    z_program_add_instruction(program, LABEL, (uint_t) (buffer), (uint_t) NULL,
                              (uint_t) NULL);
}

void z_program_add_comment(z_program_t *program, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *buffer = (char *) (malloc(100));
    vsnprintf(buffer, 99, format, args);
    va_end(args);
    z_program_add_instruction(program, COMMENT, (uint_t) (buffer), (uint_t) NULL,
                              (uint_t) NULL);
}

uint_t z_program_add_instruction(z_program_t *program, uint_t opcode, uint_t r0, uint_t r1,
                              uint_t r2) {
    //optimize compare and jmp path
    if (opcode == JMP_TRUE || opcode == JMP_NOT_TRUE) {
        int neg = opcode == JMP_NOT_TRUE;
        z_instruction_t *latest_inst = z_program_get_latest_instruction(program);
        uint_t prev_opcode = latest_inst->opcode;
        if (prev_opcode == CMP_EQUAL) {
            latest_inst->opcode = neg ? JMP_N_EQUAL : JMP_EQUAL;
            latest_inst->r2 = r1;
        } else if (prev_opcode == CMP_N_EQUAL) {
            latest_inst->opcode = neg ? JMP_EQUAL : JMP_N_EQUAL;
            latest_inst->r2 = r1;
        } else if (prev_opcode == CMP_LESS) {
            latest_inst->opcode = neg ? JMP_GREATER_OR_EQUAL : JMP_LESS;
            latest_inst->r2 = r1;
        } else if (prev_opcode == CMP_LESS_OR_EQUAL) {
            latest_inst->opcode = neg ? JMP_GREATER : JMP_LESS_OR_EQUAL;
            latest_inst->r2 = r1;
        } else if (prev_opcode == CMP_GREATER) {
            latest_inst->opcode = neg ? JMP_LESS_OR_EQUAL : JMP_GREATER;
            latest_inst->r2 = r1;
        } else if (prev_opcode == CMP_GREATER_OR_EQUAL) {
            latest_inst->opcode = neg ? JMP_LESS : JMP_GREATER_OR_EQUAL;
            latest_inst->r2 = r1;
        } else {
            goto ret;
        }
        return (uint_t) program->instructions->size - 1;
    }
    //optimize mov constant and mov path
    else if (opcode == MOV) {
        z_instruction_t *latest_inst = z_program_get_latest_instruction(program);
        uint_t prev_opcode = latest_inst->opcode;
        if ((prev_opcode == MOV_NUMBER || prev_opcode == MOV_STR) && latest_inst->r0 == r1) {
            latest_inst->r0 = r0;
            return (uint_t) program->instructions->size - 1;
        } else {
            goto ret;
        }
    }
ret:;
    return (uint_t) (arraylist_push(program->instructions,
                                 instruction_new(opcode, (uint_t) r0, (uint_t) r1, (uint_t) r2)));
}

void z_program_print(z_program_t *program) {
    for (int i = 0; i < program->instructions->size; i++) {
        instruction_print(*((z_instruction_t *) arraylist_get(program->instructions, i)));
        printf("\n");
    }
}

#endif //ZEROSCRIPT_PROGRAM_H
