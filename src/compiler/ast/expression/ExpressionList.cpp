//
// Created by onur on 18.05.2018.
//

class ExpressionList : public Expression {

public:
    vector<Expression *> *expressions = new vector<Expression *>();
    void addExpression(Expression *expr) {
        this->expressions->push_back(expr);
    }

    void print() override {
        cout << "";
        for (int i = 0; i < expressions->size(); i++) {
            this->expressions->at(expressions->size() - 1 - i)->print();
        }
        cout << "";
    }

};