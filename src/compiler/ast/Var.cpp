//
// Created by onur on 18.05.2018.
//
#include <vector>

class Var : public AST {

public:

    Var() {
        this->kind = AST_KIND_VAR;
    }

    char *identifier;
    Expression *value;

    void setIdentifier(const char *data) {
        char *temp = (char *) (malloc(sizeof(char) * strlen(data) + 1));
        memset(temp, 0, strlen(data) + 1);
        strcpy(temp, data);
        this->identifier = temp;
    }

    void print() override {
        cout << "var " << identifier << " = ";
        value->print();
    }

};
