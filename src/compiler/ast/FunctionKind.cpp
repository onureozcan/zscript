//
// Created by onur on 26.05.2018.
//

class FunctionKind : public Expression {

public:
    map<string, uint_t> *symbolTable = new map<string, uint_t>();
    map<uint_t, bool> *registerTable = new map<uint_t, bool>();
    vector<AST *> *functionsToCompile = new vector<AST *>();
    int_t isStatic = false;
    int_t isAsync = false;

    void freeRegister(uint_t index) {
        if (index > symbolTable->size()) {
            map<uint_t, bool>::iterator it = registerTable->find(index);
            if(it != registerTable->end())
            {
                (*registerTable)[index] = 1;
            }
        }
    }

    uint_t getRegister(char *ident) {
        if (!ident) {
            uint_t ret = 0;
            uint_t size = symbolTable->size() + 1;
            //get a temporary register
            for (uint_t i = 0; i < registerTable->size(); i++) {
                uint_t index = i + size;
                if (registerTable->at(index) == 1) {
                    //mark as used
                    (*registerTable)[index] = 0;
                    ret = index;
                }
            }
            if (!ret) {
                uint_t nextReg = registerTable->size() + symbolTable->size() + 1;
                (*registerTable)[nextReg] = 0;
                ret = nextReg;
            }
          //  cout << "a:" << ret << "\n";
            return ret;
        } else {
            map<string, uint_t>::const_iterator pos = symbolTable->find(ident);
            if (pos == symbolTable->end()) {
                return 0;
            } else {
                return pos->second;
            }
        }
    }

    void printSymbolTable() {
        cout << "======= symbol table ========\n";
        for (auto elem : *symbolTable) {
            cout << elem.first << ":" << elem.second << "\n";
        }
        cout << "==============================\n";
    }
    void printRegisterTable() {
        cout << "======= register table ========\n";
        for (auto elem : *registerTable) {
            cout << elem.first << ":" << elem.second << "\n";
        }
    }

};
