//
// Created by onur on 22.05.2018.
//

class Loop : public AST {

public:

    Loop(){
        this->kind = AST_KIND_LOOP;
    }
    Expression *startExpr = NULL;
    Expression *iterExpr = NULL;
    Expression *condition = NULL;
    Body *body = NULL;

    void print() override {

        cout << "loop ( ";
        if (startExpr) {
            startExpr->print();
        }
        cout << ";";
        if (condition) {
            condition->print();
        }
        cout << ";";
        if (iterExpr) {
            iterExpr->print();
        }
        cout << ") \n";
        if (body) {
            body->print();
        }

    }

};