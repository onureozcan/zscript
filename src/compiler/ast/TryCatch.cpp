//
// Created by onur on 28.09.2018.
//

class TryCatch : public AST {

public:
    TryCatch() {
        this->kind  = AST_KIND_TRY_CATCH;
    }

    Body* tryBody;
    Body* catchBody;
    Body* finallyBody;

    TerminalExpression* catchIdent;

    void print(){


    }

};