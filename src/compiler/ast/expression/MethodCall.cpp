//
// Created by onur on 18.05.2018.
//

class MethodCall : public Expression {

public:

    MethodCall(){
        this->kind = AST_KIND_METHOD_CALL;
    }

    Expression* callee;
    ExpressionList* argumentsList;

    void print() override {
        callee->print();
        printf("(");
        argumentsList->print();
        printf(")");
    }

};