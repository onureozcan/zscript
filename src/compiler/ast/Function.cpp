//
// Created by onur on 18.05.2018.
//

class Function : public FunctionKind {

public:

    Function() {
        this->kind = AST_KIND_FUNCTION;
    }

    char *identifier;
    ArgumentList *arguments = NULL;
    Body *body;

    void setIdentifier(const char *data) {
        char *temp = (char *) (malloc(sizeof(char) * strlen(data)+1));
        memset(temp, 0, strlen(data) + 1);
        strcpy(temp, data);
        this->identifier = temp;
    }

    void print() override {
        cout << "function " << identifier << "(";
        arguments->print();
        cout << ")\n";
        printSymbolTable();
        body->print();
        cout << "\nend function " << identifier << "\n";
    }


};