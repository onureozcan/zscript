//
// Created by onur on 22.05.2018.
//
class EmptyExpression : public Expression{

public:

    EmptyExpression(){
        this->kind = AST_KIND_EMPTY;
    }

    void print() override {
    }

};
