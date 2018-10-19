//
// Created by onur on 19.10.2018.
//

class BytecodeOptimizer {

public:
    arraylist_t *optimize(arraylist_t *instructions) {
        //return instructions;
        arraylist_t *ret = arraylist_new(sizeof(z_instruction_t));
        for (int_t i = 0; i < instructions->size; i++) {
            z_instruction_t *current = (z_instruction_t *) arraylist_get(instructions, i);
            z_instruction_t *next = (z_instruction_t *) arraylist_get(instructions, i + 1);
            z_instruction_t *next_next = (z_instruction_t *) arraylist_get(instructions, i + 2);

            z_instruction_t instruction = {current->opcode, current->r0, current->r1, current->r2};

            int_t opcode = current->opcode;
            // remove dead ret
            if (opcode == RETURN && next->opcode == RETURN){
                i++;
                goto add;
            }
            // remove dead mov
            if (opcode == MOV){
                if(current->r0 == current->r1) {
                    continue;
                }
            }
            // mov_n jmp -> jmp immediate
            if (opcode == MOV_NUMBER) {
                 uint_t next_opcode = next->opcode;
                 if (current->r0 == next->r1) {
                     if (next_opcode == JMP_N_EQUAL) {
                         instruction.r1 = current->r1;
                         instruction.opcode = JMP_N_EQUAL_I;
                         instruction.r0 = next->r0;
                         instruction.r2 = next->r2;
                         i++;
                     } else if (next_opcode == JMP_EQUAL) {
                         instruction.r1 = current->r1;
                         instruction.opcode = JMP_EQUAL_I;
                         instruction.r0 = next->r0;
                         instruction.r2 = next->r2;
                         i++;
                     } else if (next_opcode == JMP_LESS) {
                         instruction.r1 = current->r1;
                         instruction.opcode = JMP_LESS_I;
                         instruction.r0 = next->r0;
                         instruction.r2 = next->r2;
                         i++;
                     } else if (next_opcode == JMP_LESS_OR_EQUAL) {
                         instruction.r1 = current->r1;
                         instruction.opcode = JMP_LESS_OR_EQUAL_I;
                         instruction.r0 = next->r0;
                         instruction.r2 = next->r2;
                         i++;
                     } else if (next_opcode == JMP_GREATER) {
                         instruction.r1 = current->r1;
                         instruction.opcode = JMP_GREATER_I;
                         instruction.r0 = next->r0;
                         instruction.r2 = next->r2;
                         i++;
                     } else if (next_opcode == JMP_GREATER_OR_EQUAL) {
                         instruction.r1 = current->r1;
                         instruction.opcode = JMP_GREATER_OR_EQUAL_I;
                         instruction.r0 = next->r0;
                         instruction.r2 = next->r2;
                         i++;
                     }
                 }
             }
            // compare and mov
            //	cne   $4     $9     $10
            //	mov   $5     $10
            if (opcode >= CMP_EQUAL && opcode <= CMP_GREATER_OR_EQUAL) {
                uint_t next_opcode = next->opcode;
                if(next->r1 == current->r2){
                    i++;
                    instruction.r2 = next->r1;
                    goto add;
                }
            }
            // compare and jump
            if (opcode >= CMP_EQUAL && opcode <= CMP_GREATER_OR_EQUAL) {
                uint_t next_opcode = next->opcode;
                int neg = next_opcode == JMP_NOT_TRUE;
                if (next_opcode != JMP_NOT_TRUE && next_opcode != JMP_TRUE) goto add;
                if (opcode == CMP_EQUAL) {
                    instruction.opcode = neg ? JMP_N_EQUAL : JMP_EQUAL;
                    instruction.r2 = next->r1;
                    i++;
                } else if (opcode == CMP_N_EQUAL) {
                    instruction.opcode = neg ? JMP_EQUAL : JMP_N_EQUAL;
                    instruction.r2 = next->r1;
                    i++;
                } else if (opcode == CMP_LESS) {
                    instruction.opcode = neg ? JMP_GREATER_OR_EQUAL : JMP_LESS;
                    instruction.r2 = next->r1;
                    i++;
                } else if (opcode == CMP_LESS_OR_EQUAL) {
                    instruction.opcode = neg ? JMP_GREATER : JMP_LESS_OR_EQUAL;
                    instruction.r2 = next->r1;
                    i++;
                } else if (opcode == CMP_GREATER) {
                    instruction.opcode = neg ? JMP_LESS_OR_EQUAL : JMP_GREATER;
                    instruction.r2 = next->r1;
                    i++;
                } else if (opcode == CMP_GREATER_OR_EQUAL) {
                    instruction.opcode = neg ? JMP_LESS : JMP_GREATER_OR_EQUAL;
                    instruction.r2 = next->r1;
                    i++;
                }
            }

        add:
            arraylist_push(ret, &instruction);
        }
        int_t optimized_count = instructions->size - ret->size;
        if (optimized_count == 0) {
            return ret;
        }
        arraylist_free(instructions);
        return optimize(ret);
    }
};