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
    bool isStatic = false;
    bool isPrivate = false;

    void setIdentifier(const char *data) {
        char *temp = (char *) (malloc(sizeof(char) * strlen(data) + 1));
        memset(temp, 0, strlen(data) );
        strcpy(temp, data);
        this->identifier = temp;
    }

    void print() override {
        if(isStatic){
            cout << "static ";
        }
        if(isPrivate){
            cout << "private ";
        }
        cout << "var " << identifier << " = ";
        value->print();
    }

};
