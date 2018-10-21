//
// Created by onur on 18.05.2018.
//

class TerminalExpression : public Expression {

private:

    static TerminalExpression *getOne(const char *data) {
        TerminalExpression *self = new TerminalExpression();
        char *temp = (char *) (malloc(sizeof(char) * strlen(data) + 1));
        memset(temp, 0, strlen(data) + 1);
        strcpy(temp, data);
        self->data = temp;
        return self;
    }

public:
    static const int TYPE_IDENTIFIER = 0;
    static const int TYPE_NUMBER = 1;
    static const int TYPE_STRING = 2;

    TerminalExpression() {
        kind = AST::AST_KIND_TERMINAL;
    }

    static TerminalExpression *identifier(const char *data) {
        TerminalExpression *self = getOne(data);
        self->type = TYPE_IDENTIFIER;
        return self;
    }

    static TerminalExpression *number(const char *data) {
        TerminalExpression *self = getOne(data);
        self->type = TYPE_NUMBER;
        return self;
    }

    static TerminalExpression *stringWithoutTrim(const char *data) {
        TerminalExpression *self = getOne(data);
        self->type = TYPE_STRING;
        return self;
    }

    static TerminalExpression *string(const char *data) {
        TerminalExpression *self = getOne(data + 1);
        self->type = TYPE_STRING;
        self->data[strlen(self->data) - 1] = 0;
        return self;
    }

    int type;
    char *data;

    void print() override {
        printf(" %s ", data);
    }

private:

    const char *typeToStr(int type) {
        if (type == TYPE_IDENTIFIER) return "IDENT";
        if (type == TYPE_NUMBER) return "NUM";
        if (type == TYPE_STRING) return "STR";
    }

};
