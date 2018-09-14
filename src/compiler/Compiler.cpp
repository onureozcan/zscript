//
// Created by onur on 26.05.2018.
//

#include "compiler.h"

class Compiler {

    Program *program = new Program();
    stack<FunctionKind *> *functionsStack = new stack<FunctionKind *>();
    stack<char *> *loopEndLabelsStack = new stack<char *>();
    stack<char *> *loopStartLabelsStack = new stack<char *>();

    int labelCount = 0;

    int getRegister(char *ident) {
        return functionsStack->top()->getRegister(ident);
    }

    void freeRegister(int index) {
        functionsStack->top()->freeRegister(index);
    }

    map<string, int> *getCurrentSymbolTable() {
        return functionsStack->top()->symbolTable;
    };
    char *bytes = NULL;
    size_t len = 0;
public :
    Compiler(ClassDeclaration *ast) {
        AddressCalculator *addressCalculator = new AddressCalculator(ast);
        compileClass((ast));
        //program->print();
        Assembler assembler;
        bytes = assembler.toBytes(program, &len);
        //ast->print();
    }

    char *toBytes(size_t *len) {
        *len = this->len;
        return bytes;
    }

    uint_t compileExpression(Expression *pExpr) {
        ExpressionCompiler *expressionCompiler = new ExpressionCompiler(program, functionsStack->top());
        uint_t ret = expressionCompiler->compileExpression(pExpr);
        delete (expressionCompiler);
        return ret;
    }

    void compileFunction(Function *func) {
        program->addLabel("%s", func->identifier);
        uint_t index = program->addInstruction(FFRAME, (uint_t) NULL, (uint_t) NULL,
                                               (uint_t) NULL);
        functionsStack->push(func);
        int i = (int) (func->arguments->identifiers->size()) - 1;
        for (; i > -1; i--) {
            char *ident = func->arguments->identifiers->at(i)->data;
            uint_t identReg = (uint_t) (func->getRegister(ident));
            program->addComment( "pop arg %i (%s)", i, ident);
            program->addInstruction(POP, identReg, (uint_t) NULL,
                                      (uint_t) NULL);
        }
        program->addComment( "mov function ptr to register for %s", func->identifier);
        program->addInstruction( MOV_FNC, getRegister(func->identifier),
                                 (uint64_t) func->identifier, NULL);
        compileBody(func->body);
        program->addInstruction(RETURN, (uint_t) NULL, (uint_t) NULL, (uint_t) NULL);
        compileRemainingFunctions();
        uint_t locals_count = func->registerTable->size() + func->symbolTable->size();
        //set locals count
        ((z_instruction_t *) arraylist_get(program->instructions, index))->r0 = locals_count;
        ((z_instruction_t *) arraylist_get(program->instructions, index))->r1 = (uint_t)(func->symbolTable);
        functionsStack->pop();
    }

    void compileLoop(Loop *pLoop) {
        char *lstart_label = static_cast<char *>(malloc(100));
        sprintf(lstart_label, ".lstart_%d", ++labelCount);
        char *lend_label = static_cast<char *>(malloc(100));
        sprintf(lend_label, ".lend_%d", ++labelCount);
        loopEndLabelsStack->push(lend_label);
        loopStartLabelsStack->push(lstart_label);
        char *lcond_label = static_cast<char *>(malloc(100));
        sprintf(lcond_label, ".lcond_%d", ++labelCount);

        if (pLoop->startExpr) {
            program->addComment( "loop start expression");
            uint_t startReg = compileExpression(pLoop->startExpr);
            freeRegister(startReg);
        }
        program->addComment( "jump to condition");
        program->addInstruction(JMP, (uint_t) lcond_label, (uint_t) NULL,
                                  (uint_t) NULL);
        program->addLabel( lstart_label);
        program->addComment( "loop body");
        compileBody(pLoop->body);
        if (pLoop->iterExpr) {
            program->addComment( "loop iteration");
            uint_t iterReg = compileExpression(pLoop->iterExpr);
            freeRegister(iterReg);
        }
        program->addLabel( lcond_label);
        if (pLoop->condition) {
            program->addComment( "loop condition");
            uint_t condReg = compileExpression(pLoop->condition);
            program->addComment( "jump to the end if loop condition fails");

            program->addInstruction(JMP_TRUE, condReg, (uint_t) lstart_label,
                                      (uint_t) NULL);
            freeRegister(condReg);
        }
        program->addLabel( lend_label);
        loopEndLabelsStack->pop();
    }

    void compileConditional(Conditional *pConditional) {
        char *if_fail_label = static_cast<char *>(malloc(100));
        sprintf(if_fail_label, ".else_%d", ++labelCount);
        char *con_end_label = static_cast<char *>(malloc(100));
        sprintf(con_end_label, ".end_cond_%d", ++labelCount);
        program->addComment( "if condition");
        uint_t cond_reg = compileExpression(pConditional->condition);
        program->addComment( "jump if condition is not satisfied");
        program->addInstruction(JMP_NOT_TRUE, (uint_t) cond_reg, (uint_t) if_fail_label,
                                  (uint_t) NULL);
        freeRegister(cond_reg);
        compileBody(pConditional->body);
        if (pConditional->elseBody) {
            program->addInstruction( NOP, (uint_t) NULL, (uint_t) NULL, (uint_t) NULL);
            program->addInstruction( JMP, (uint_t) con_end_label, (uint_t) NULL,
                                      (uint_t) NULL);
        }
        program->addLabel( if_fail_label);
        if (pConditional->elseBody) {
            compileBody(pConditional->elseBody);
        }
        program->addLabel( con_end_label);
    }

    void compileVar(Var *pVar) {
        uint_t reg = (uint_t) getRegister(pVar->identifier);
        uint_t valueReg = compileExpression(pVar->value);
        program->addInstruction( MOV, reg, valueReg, (uint_t) NULL);
        freeRegister(valueReg);
    }

    void compileStmt(Statement *statement) {
        if (statement->hasBreak) {
            program->addComment( "break ");
            char *lend_label = loopEndLabelsStack->top();
            program->addInstruction( JMP, (uint_t) lend_label,
                                      (uint_t) NULL, (uint_t) NULL);
        }
        AST *stmt = statement->stmt;
        if (stmt->kind == AST::AST_KIND_FUNCTION) {
            addFunctionToCompileLater(dynamic_cast<Function *>(stmt));
        } else if (stmt->kind == AST::AST_KIND_VAR) {
            program->addComment("init variable %s", dynamic_cast<Var *>(stmt)->identifier);
            compileVar(dynamic_cast<Var *>(stmt));
        } else if (stmt->kind == AST::AST_KIND_LOOP) {
            compileLoop(dynamic_cast<Loop *>(stmt));
        } else if (stmt->kind == AST::AST_KIND_BODY) {
            compileBody(dynamic_cast<Body *>(stmt));
        } else if (stmt->kind == AST::AST_KIND_CONDITIONAL) {
            compileConditional(dynamic_cast<Conditional *>(stmt));
        } else if (dynamic_cast<Expression *>(stmt)) {
            uint_t reg = compileExpression(dynamic_cast<Expression *>(stmt));
            if (statement->hasReturn) {
                program->addComment( " return %d", reg);
                program->addInstruction( RETURN, reg, (uint_t) NULL, (uint_t) NULL);
            }
            freeRegister(reg);
        }
    }

    void compileBody(Body *pBody) {
        vector<Statement *> *statements = pBody->getStatements();
        //hoisting
        for (int i = 0; i < statements->size(); i++) {
            AST *stmt = statements->at(i)->stmt;
            if (stmt->kind == AST::AST_KIND_FUNCTION) {
                Function *func = dynamic_cast<Function *>(stmt);
                program->addComment( "mov function ptr to register for %s", func->identifier);
                program->addInstruction(MOV_FNC, (uint_t)(getRegister(func->identifier)),
                                        (uint_t) func->identifier, NULL);
            }

        }
        //actual compiling
        for (int i = 0; i < statements->size(); i++) {
            Statement *stmt = statements->at(i);
            compileStmt(stmt);
        }
    }

    void addFunctionToCompileLater(Function *fnc) {
        functionsStack->top()->functionsToCompile->push_back(fnc);
    }

    void compileRemainingFunctions() {
        vector<AST *> *functionsToCompile = functionsStack->top()->functionsToCompile;
        for (int i = 0; i < functionsToCompile->size(); i++) {
            compileFunction(dynamic_cast<Function *>(functionsToCompile->at(i)));
        }
        functionsStack->top()->functionsToCompile->clear();
    }

    void compileClass(ClassDeclaration *cls) {
        program->addLabel( "%s", cls->identifier);
        uint_t index = program->addInstruction( FFRAME, NULL, NULL, NULL);
        functionsStack->push(cls);
        compileBody(cls->body);
        program->addInstruction( RETURN, (uint_t) NULL, (uint_t) NULL, (uint_t) NULL);
        compileRemainingFunctions();
        uint_t locals_count = cls->registerTable->size() + cls->symbolTable->size();
        //set args of FFRAME
        ((z_instruction_t *) arraylist_get(program->instructions, index))->r0 = locals_count;
        ((z_instruction_t *) arraylist_get(program->instructions, index))->r1 = (uint_t)(cls->symbolTable);
        functionsStack->pop();
    }

};