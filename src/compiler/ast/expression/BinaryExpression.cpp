//
// Created by onur on 18.05.2018.
//

class BinaryExpression : public Expression {
public:

    BinaryExpression() {
        this->kind = AST_KIND_BINARY;
    }

    char *op;
    Expression *left;
    Expression *right;

    static BinaryExpression *create(const char *op, stack<AST *> *astStack) {
        BinaryExpression *self = new BinaryExpression();
        self->setOp(op);
        Expression *left = dynamic_cast<Expression *>(astStack->top());
        astStack->pop();
        Expression *right = dynamic_cast<Expression *>(astStack->top());
        astStack->pop();
        self->left = right;
        self->right = left;
        return self;
    }

    void setOp(const char *data) {
        char *temp = (char *) (malloc(sizeof(char) * strlen(data) + 1));
        strcpy(temp, data);
        this->op = temp;
    }

    void print() override {
        left->print();
        printf(" %s ", op);
        right->print();
    }
};