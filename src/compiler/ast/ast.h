//
// Created by onur on 18.05.2018.
//

#ifndef ZEROSCRIPT_AST_H
#define ZEROSCRIPT_AST_H

class AST {
public :
    char* filename;
    int line_number;
    int kind = -1;

    static const int AST_KIND_TERMINAL = 0;
    static const int AST_KIND_PREFIX = 1;
    static const int AST_KIND_POSTFIX = 2;
    static const int AST_KIND_METHOD_CALL = 3;
    static const int AST_KIND_EXPRESSION_LIST = 4;
    static const int AST_KIND_EMPTY = 5;
    static const int AST_KIND_BINARY = 6;
    static const int AST_KIND_BODY = 7;
    static const int AST_KIND_CLASS = 8;
    static const int AST_KIND_FUNCTION = 9;
    static const int AST_KIND_LOOP = 10;
    static const int AST_KIND_VAR = 11;
    static const int AST_KIND_CONDITIONAL = 12;
    static const int AST_KIND_SWITCH_CASE = 13;

    virtual void print() = 0;

};

#include "expression/expression.h"
#include "Statement.cpp"
#include "Body.cpp"
#include "ArgumentsList.cpp"
#include "FunctionKind.cpp"
#include "ClassDeclaration.cpp"
#include "Function.cpp"
#include "Conditional.cpp"
#include "Var.cpp"
#include "Loop.cpp"
#include "SwitchCase.cpp"

#endif //ZEROSCRIPT_AST_H
