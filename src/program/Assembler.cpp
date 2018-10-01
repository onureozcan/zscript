//
// Created by onur on 06.06.2018.
//

class Assembler {

    char *bytes = NULL;
    uint_t pos = 0;

    struct cmp_str {
        bool operator()(char const *a, char const *b) {
            return std::strcmp(a, b) < 0;
        }
    };

    map<char *, uint_t, cmp_str> label_position_map = map<char *, uint_t, cmp_str>();

    uint_t addData(char *data, uint_t len) {
        bytes = static_cast<char *>(realloc(bytes, pos + len));
        memcpy(bytes + pos, data, len);
        pos += len;
        return pos - len;
    }

    void assemble(Program *program) {
        uint_t sizeof_constants = 0;
        addData((char *) &sizeof_constants, sizeof(uint_t));

        //place and calculate strings aka constants
        int_t ip_of_static_const = addData(" ", sizeof(int_t));
        for (int i = 0; i < program->instructions->size; i++) {
            z_instruction_t instruction = *((z_instruction_t *) arraylist_get(program->instructions, i));
           if (instruction.opcode == IMPORT_CLS) {
                char* import = (char *) instruction.r0;
                char* as = (char *) instruction.r1;
                z_instruction_t *instruction_ptr = ((z_instruction_t *) arraylist_get(program->instructions, i));
                instruction_ptr->r1 = (addData(as, (uint_t) (strlen(as) + 1)));
                instruction_ptr->r0 = (addData(import, (uint_t) (strlen(import) + 1)));
            } else if (instruction.opcode == MOV_STR || instruction.opcode == GET_FIELD_IMMEDIATE) {
                char *str = (char *) instruction.r1;
                z_instruction_t *instruction_ptr = ((z_instruction_t *) arraylist_get(program->instructions, i));
                instruction_ptr->r1 = (addData(str, (uint_t) (strlen(str) + 1)));
            } else if (instruction.opcode == FFRAME) {
                z_instruction_t *instruction_ptr = ((z_instruction_t *) arraylist_get(program->instructions, i));
                map<string, int> symbolTable = *((map<string, int> *) (instruction.r1));

                typedef std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)> Comparator;

                // Defining a lambda function to compare two pairs. It will compare two pairs using second field
                Comparator compFunctor =
                        [](std::pair<std::string, int> elem1, std::pair<std::string, int> elem2) {
                            return elem1.second < elem2.second;
                        };

                // Declaring a set that will store the pairs using above comparision logic
                std::set<std::pair<std::string, int>, Comparator> setOfWords(
                        symbolTable.begin(), symbolTable.end(), compFunctor);
                int i = 0;
                int_t size = symbolTable.size();
                instruction_ptr->r1 = addData((char *) &size, sizeof(int_t));
                // Iterate over a set using range base for loop
                // It will display the items in sorted order of properties
                for (std::pair<std::string, int> pair : setOfWords) {
                    char *str = (char *) pair.first.data();
                    uint_t pos = (addData(str, (uint_t) (strlen(str) + 1)));
                    i++;
                }
                instruction_ptr->r2 = i;
            }
        }

        //calculate labels
        sizeof_constants = pos;
        uint_t lpos = 0;
        for (int i = 0; i < program->instructions->size; i++) {
            z_instruction_t instruction = *((z_instruction_t *) arraylist_get(program->instructions, i));
            if (instruction.opcode == LABEL) {
                // cout << " PUT: `" << (char *) instruction.r0 << "` " << ftell(o_file) + lpos << "\n";
                label_position_map[(char *) instruction.r0] = sizeof_constants + lpos;
                //put position of static constructor
                if (strcmp((char *) instruction.r0, "__static__constructor__") == 0) {
                    uint_t *second_int = (uint_t *) (bytes + sizeof(int_t));
                    *second_int = sizeof_constants + lpos;
                }
            } else if (instruction.opcode != COMMENT) {
                lpos += sizeof(uint_t) * 4;
            }
        }
        //assemble
        for (uint_t i = 0; i < program->instructions->size; i++) {
            z_instruction_t instruction = *((z_instruction_t *) arraylist_get(program->instructions, i));
            if (instruction.opcode != COMMENT && instruction.opcode != LABEL) {
                if (instruction.opcode >= JMP_LESS && instruction.opcode <= JMP_N_EQUAL) {
                    instruction.r2 = label_position_map[(char *) instruction.r2];
                } else if (instruction.opcode == JMP_NOT_TRUE || instruction.opcode == JMP_TRUE) {
                    instruction.r1 = label_position_map[(char *) instruction.r1];
                } else if (instruction.opcode == JMP) {
                    instruction.r0 = label_position_map[(char *) instruction.r0];
                } else if (instruction.opcode == MOV_FNC) {
                    instruction.r1 = label_position_map[(char *) instruction.r1];
                } else if (instruction.opcode == SET_CATCH) {
                    instruction.r0 = label_position_map[(char *) instruction.r0];
                }
                uint_t opcode = (instruction.opcode);
                uint_t r0 = (instruction.r0);
                uint_t r1 = (instruction.r1);
                uint_t r2 = (instruction.r2);

                addData((char *) &opcode, sizeof(uint_t));
                addData((char *) &r0, sizeof(uint_t));
                if (instruction.opcode == MOV_NUMBER) {
#ifdef FLOAT_SUPPORT
                    FLOAT num = 0;
#else
                    int_t num = 0;
#endif
                    char *num_str = (char *) instruction.r1;
                    if (strcmp(num_str, "true") == 0) {
                        num = 1;
                    } else if (strcmp(num_str, "false") == 0) {
                        num = 0;
                    } else {
#ifdef FLOAT_SUPPORT
                        num = atof(num_str);
#else
                        num = atoi(num_str);
#endif
                    }
                    addData((char *) &num, sizeof(int_t));
                } else {
                    addData((char *) &r1, sizeof(int_t));
                }
                addData((char *) &r2, sizeof(uint_t));
            }
        }
        uint_t *fist_int = (uint_t *) (bytes);
        *fist_int = sizeof_constants;
    }

public:
    char *toBytes(Program *z_program, size_t *len) {
        assemble(z_program);
        *len = pos;
        return bytes;
    }
};

