//
// Created by onur on 26.05.2018.
//

class FunctionKind : public Expression {

public:
    map<string, int> *symbolTable = new map<string, int>();
    map<int, bool> *registerTable = new map<int, bool>();
    vector<AST *> *functionsToCompile = new vector<AST *>();
    int_t isStatic;

    void freeRegister(int index) {
        if (index > symbolTable->size()) {
           // cout << "f:" << index << "\n";
            (*registerTable)[index] = 1;
        }
    }

    int getRegister(char *ident) {
        if (!ident) {
            int ret = 0;
            int size = (int) symbolTable->size() + 1;
            //get a temporary register
            for (int i = 0; i < registerTable->size(); i++) {
                int index = i + size;
                if (registerTable->at(index) == 1) {
                    //mark as used
                    (*registerTable)[index] = 0;
                    ret = index;
                }
            }
            if (!ret) {
                int nextReg = registerTable->size() + symbolTable->size() + 1;
                (*registerTable)[nextReg] = 0;
                ret = nextReg;
            }
          //  cout << "a:" << ret << "\n";
            return ret;
        } else {
            map<string, int>::const_iterator pos = symbolTable->find(ident);
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
        cout << "\n";
    }

};
