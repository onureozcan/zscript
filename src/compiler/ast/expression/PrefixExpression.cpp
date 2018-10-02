//
// Created by onur on 18.05.2018.
//

class PrefixExpression : public Expression {
public:
    char *op;
    Expression *expr;

    PrefixExpression() {
        this->kind = AST_KIND_PREFIX;
    }

    static PrefixExpression *create(const char *op, stack<AST *> *astStack) {
        PrefixExpression *self = new PrefixExpression();
        self->setOp(op);
        Expression *expr = dynamic_cast<Expression *>(astStack->top());
        astStack->pop();
        self->expr = expr;
        return self;
    }

    void setOp(const char *data) {
        char *temp = (char *) (malloc(sizeof(char) * strlen(data) + 1));
        strcpy(temp, data);
        this->op = temp;
    }

    void print() override {
        printf(" %s ", op);
        expr->print();
    }
};