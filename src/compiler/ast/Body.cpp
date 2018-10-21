//
// Created by onur on 18.05.2018.
//
#include <vector>

class Body : public AST {

public:

    Body() {
        this->kind = AST_KIND_BODY;
    }

    vector<Statement *> *statements = new vector<Statement *>();

    vector<Statement *>* getStatements(){
        /*vector<Statement*> *list = new vector<Statement*>();
        for (int i = 0; i < statements->size(); i++) {
            Statement *stmt = statements->at(statements->size() - i - 1);
            if (stmt == NULL) continue;
            list->push_back(stmt);
        }*/
        return statements;
    }

    void print() override {
        cout << "{\n";
        int count = 0;
        for (int i = 0; i < statements->size(); i++) {
            AST *stmt = statements->at(i);
            if (stmt == NULL) continue;
            count++;
            cout << "\n" << "";
            stmt->print();
        }
        cout << "}\n";
    }

};
