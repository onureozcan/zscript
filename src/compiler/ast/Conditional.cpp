//
// Created by onur on 22.05.2018.
//

class Conditional : public AST {

public:

    Conditional(){
        this->kind = AST_KIND_CONDITIONAL;
    }

    Expression *condition = NULL;
    Body *body = NULL;
    Body *elseBody = NULL;

    void print() override {

        cout << "if (";
        condition->print();
        cout << ")";
        body->print();
        if (elseBody) {
            cout << "else ";
            elseBody->print();
        }
    }

};