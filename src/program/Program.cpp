//
// Created by onur on 10.07.2018.
//

#include "instruction.h"

class Program {
public:
    arraylist_t *instructions;
    z_instruction_t *latestInstruction = NULL;

    Program() {
        this->instructions = arraylist_new(sizeof(z_instruction_t));
    }

    void addLabel(const char *format, ...) {
        va_list args;
        va_start(args, format);
        char *buffer = (char *) (malloc(100));
        vsnprintf(buffer, 99, format, args);
        va_end(args);
        addInstruction(LABEL, (uint_t) (buffer), (uint_t) NULL,
                       (uint_t) NULL);
    }

    void addComment(const char *format, ...) {
        va_list args;
        va_start(args, format);
        char *buffer = (char *) (malloc(100));
        vsnprintf(buffer, 99, format, args);
        va_end(args);
        addInstruction(COMMENT, (uint_t) (buffer), (uint_t) NULL,
                       (uint_t) NULL);
    }

    z_instruction_t *getLatestInstruction() {
        for(int_t i = this->instructions->size-1;i>-1;i--){
            latestInstruction = static_cast<z_instruction_t *>(arraylist_get(this->instructions,
                                                                             i));
            if (latestInstruction->opcode != COMMENT && latestInstruction->opcode != LABEL) {
                return latestInstruction;
            }
        }
        return NULL;
    }

    uint_t addInstruction(uint_t opcode, uint_t r0, uint_t r1, uint_t r2) {
        //optimize compare and jmp path
        if (opcode == JMP_TRUE || opcode == JMP_NOT_TRUE) {
            int neg = opcode == JMP_NOT_TRUE;
            z_instruction_t *latest_inst = getLatestInstruction();
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
            return (uint_t) this->instructions->size - 1;
        }
            //optimize mov constant and mov path
        else if (opcode == MOV) {
            z_instruction_t *latest_inst = getLatestInstruction();
            uint_t prev_opcode = latest_inst->opcode;
            if ((prev_opcode == MOV_NUMBER || prev_opcode == MOV_STR) && latest_inst->r0 == r1) {
                latest_inst->r0 = r0;
                return (uint_t) this->instructions->size - 1;
            } else {
                goto ret;
            }
        }
    ret:;
        uint_t retVal = (arraylist_push(this->instructions,
                                        instruction_new(opcode, (uint_t) r0, (uint_t) r1, (uint_t) r2)));
        return retVal;
    }

    void print() {
        for (int i = 0; i < this->instructions->size; i++) {
            instruction_print(*((z_instruction_t *) arraylist_get(this->instructions, i)));
            printf("\n");
        }
    }
};