//
// Created by onur on 18.05.2018.
//

class PostfixExpression : public Expression {
public:
    char *op;
    Expression* expr;

    PostfixExpression(){
        this->kind = AST_KIND_POSTFIX;
    }

    static PostfixExpression *create(const char *op, stack<AST*> *astStack) {
        PostfixExpression *self = new PostfixExpression();
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
        expr->print();
        printf(" %s ", op);
    }
};