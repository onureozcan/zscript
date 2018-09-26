//
// Created by onur on 18.05.2018.
//

class ClassDeclaration : public FunctionKind {

public:
    char *identifier = NULL;
    ArgumentList *arguments = NULL;
    Body *body = NULL;
    //imported classes map
    map<string,string> importsMap = map<string,string>();

    void setIdentifier(const char *data) {
        char *temp = (char *) (malloc(sizeof(char) * strlen(data) + 1));
        memset(temp, 0, strlen(data) + 1);
        strcpy(temp, data);
        this->identifier = temp;
    }

    void print() override {
        cout << "class " << identifier << "(";
        if (arguments)
            arguments->print();
        cout << ")\n";
        printSymbolTable();
        body->print();
        cout << "\nend class " << identifier << "\n";
    }


};