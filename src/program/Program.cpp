//
// Created by onur on 10.07.2018.
//

#include "instruction.h"
#include "BytecodeOptimizer.cpp"

class Program {
public:
    arraylist_t *instructions;
    BytecodeOptimizer optimizer;

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

    }


    int_t addInstruction(uint_t opcode, uint_t r0, uint_t r1, uint_t r2) {
        int_t retVal = (arraylist_push(this->instructions,
                                        instruction_new(opcode, (uint_t) r0, (uint_t) r1, (uint_t) r2)));
        return retVal;
    }

    void print() {
        for (int i = 0; i < this->instructions->size; i++) {
            instruction_print(*((z_instruction_t *) arraylist_get(this->instructions, i)));
            printf("\n");
        }
    }

    void optimize() {
        this->instructions = optimizer.optimize(this->instructions);
    }
};