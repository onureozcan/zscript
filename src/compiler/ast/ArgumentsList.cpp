//
// Created by onur on 22.05.2018.
//

class ArgumentList : public Expression {
public:
    vector<TerminalExpression *> *identifiers = new vector<TerminalExpression *>();
    void addIdent(TerminalExpression *ident) {
        this->identifiers->push_back(ident);
    }

    void print() override {
        cout << "";
        for (int i = 0; i < identifiers->size(); i++) {
            this->identifiers->at(identifiers->size() - 1 - i)->print();
        }
        cout << "";
    }

};