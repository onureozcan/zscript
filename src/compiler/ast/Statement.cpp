//
// Created by onur on 18.05.2018.
//
class Statement : public AST {

public:

    AST* stmt;
    bool hasReturn;
    bool hasBreak;
    bool hasContinue;


    void print() override {
        if(hasReturn)
            cout << "return ";
        if(hasBreak)
            cout << "break ";
        if(hasContinue)
            cout << "continue ";
        stmt->print();
    }

};
