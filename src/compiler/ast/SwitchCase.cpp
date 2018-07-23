//
// Created by onur on 16.07.2018.
//

class SwitchCase : public AST {

public:

    SwitchCase(){
        this->kind = AST_KIND_SWITCH_CASE;
    }

    Expression* test;
    vector<Expression*> *expressions = new vector<Expression*>();
    vector<Body*> *bodies = new vector<Body*>();

    void print() override {


    }


};