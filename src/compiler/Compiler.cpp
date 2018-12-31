//
// Created by onur on 26.05.2018.
//

#include "compiler.h"

class Compiler {

    Program *program = new Program();
    stack<FunctionKind *> *functionsStack = new stack<FunctionKind *>();
    stack<char *> *loopEndLabelsStack = new stack<char *>();
    stack<char *> *loopStartLabelsStack = new stack<char *>();
    // when a return inside try is encountered, prepend finally body
    //TODO: crashes if i use vector instead of custom written arraylist. why?
    arraylist_t *finallyBlocksToPrependReturn = arraylist_new(sizeof(TryCatch));
    vector<char *> *compiledStaticFunctions = new vector<char *>();
    ClassDeclaration *cls;
    int labelCount = 0;

    uint_t getRegister(char *ident) {
        return functionsStack->top()->getRegister(ident);
    }

    void freeRegister(uint_t index) {
        functionsStack->top()->freeRegister(index);
    }

    map<string, uint_t> *getCurrentSymbolTable() {
        return functionsStack->top()->symbolTable;
    };
    char *bytes = NULL;
    size_t len = 0;
public :
    Compiler(ClassDeclaration *ast) {
        this->cls = ast;
        AddressCalculator *addressCalculator = new AddressCalculator(ast);
        //ast->print();
        compileClass((ast));
        program->optimize();
        //program->print();
        //exit(0);
        Assembler assembler;
        bytes = assembler.toBytes(program, &len);
        arraylist_free(program->instructions);
        delete(addressCalculator);
    }

    char *toBytes(size_t *len) {
        *len = this->len;
        return bytes;
    }

    uint_t compileExpression(Expression *pExpr, uint_t requestedDestinationRegister = 0) {
        ExpressionCompiler *expressionCompiler = new ExpressionCompiler(program, functionsStack->top());
        uint_t ret = expressionCompiler->compileExpression(pExpr, requestedDestinationRegister);
        delete (expressionCompiler);
        return ret;
    }

    void compileFunction(Function *func) {
        char* endLabel = static_cast<char *>(malloc(100));
        snprintf(endLabel,100,"end%s",func->identifier);
        program->addLabel("%s", func->identifier);
        uint_t index = program->addInstruction(FFRAME, (uint_t) NULL, (uint_t) NULL,
                                               (uint_t) NULL);
        functionsStack->push(func);
        /*int i = (int) (func->arguments->identifiers->size()) - 1;
        for (i = 0; i < (int) (func->arguments->identifiers->size()); i++) {
            char *ident = func->arguments->identifiers->at(i)->data;
            uint_t identReg = (uint_t) (func->getRegister(ident));
            program->addComment("pop arg %i (%s)", i, ident);
            program->addInstruction(POP, identReg, (uint_t) NULL,
                                    (uint_t) NULL);
        }*/
        program->addComment("mov function ptr to register for %s", func->identifier);
        program->addInstruction(MOV_FNC, (uint_t) getRegister(func->identifier),
                                (uint64_t) func->identifier, static_cast<uint_t>(func->isAsync));
        if (strcmp(func->identifier, "__static__constructor__") == 0) {
            //if this is the static constructor, add all other static functions and imports to the __static__ variable
            for (auto elem : cls->importsMap) {
                char *first = (char *) malloc(strlen(elem.first.c_str()) + 1);
                char *second = (char *) malloc(strlen(elem.second.c_str()) + 1);
                strcpy(first, elem.first.c_str());
                strcpy(second, elem.second.c_str());
                //add import instruction
                program->addInstruction(IMPORT_CLS, (uint_t) first, (uint_t) second, NULL);
            }
            uint_t staticsRegister = static_cast<uint_t>(getRegister(NULL));
            program->addInstruction(GET_FIELD_IMMEDIATE, 1, (uint_t) "__static__", staticsRegister);
            uint_t fncNameRegister = static_cast<uint_t>(getRegister(NULL));
            uint_t fncRefRegister = static_cast<uint_t>(getRegister(NULL));
            for (int_t i = 0; i < compiledStaticFunctions->size(); i++) {
                char *fncName = compiledStaticFunctions->at(i);
                program->addComment("mov function ptr to register for %s", fncName);
                program->addInstruction(MOV_FNC, (uint_t) fncRefRegister,
                                        (uint_t) fncName, NULL);
                program->addInstruction(MOV_STR, fncNameRegister, (uint_t) fncName, 0);
                program->addInstruction(SET_FIELD, staticsRegister, fncNameRegister, fncRefRegister);
            }
        } else if (func->isStatic) {
            //if this is a static method, register it to further usage
            compiledStaticFunctions->push_back(func->identifier);
        }
        compileBody(func->body);
        program->addInstruction(RETURN, (uint_t) NULL, (uint_t) NULL, (uint_t) NULL);
        program->addLabel("end%s",func->identifier);
        compileRemainingFunctions();
        uint_t locals_count = func->registerTable->size() + func->symbolTable->size();
        //set locals count
        ((z_instruction_t *) arraylist_get(program->instructions, index))->r0 = locals_count;
        ((z_instruction_t *) arraylist_get(program->instructions, index))->r1 = (uint_t) (func);
        ((z_instruction_t *) arraylist_get(program->instructions, index))->r2 = (uint_t) (endLabel);
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
            program->addComment("loop start expression");
            uint_t startReg = compileExpression(pLoop->startExpr);
            freeRegister(startReg);
        }
        program->addComment("jump to condition");
        program->addInstruction(JMP, (uint_t) lcond_label, (uint_t) NULL,
                                (uint_t) NULL);
        program->addLabel(lstart_label);
        program->addComment("loop body");
        compileBody(pLoop->body);
        if (pLoop->iterExpr) {
            program->addComment("loop iteration");
            uint_t iterReg = compileExpression(pLoop->iterExpr);
            freeRegister(iterReg);
        }
        program->addLabel(lcond_label);
        if (pLoop->condition) {
            program->addComment("loop condition");
            uint_t condReg = compileExpression(pLoop->condition);
            program->addComment("jump to the end if loop condition fails");

            program->addInstruction(JMP_TRUE, condReg, (uint_t) lstart_label,
                                    (uint_t) NULL);
            freeRegister(condReg);
        }
        program->addLabel(lend_label);
        loopEndLabelsStack->pop();
    }

    void compileConditional(Conditional *pConditional) {
        char *if_fail_label = static_cast<char *>(malloc(100));
        sprintf(if_fail_label, ".else_%d", ++labelCount);
        char *con_end_label = static_cast<char *>(malloc(100));
        sprintf(con_end_label, ".end_cond_%d", ++labelCount);
        program->addComment("if condition");
        uint_t cond_reg = compileExpression(pConditional->condition);
        program->addComment("jump if condition is not satisfied");
        program->addInstruction(JMP_NOT_TRUE, (uint_t) cond_reg, (uint_t) if_fail_label,
                                (uint_t) NULL);
        freeRegister(cond_reg);
        compileBody(pConditional->body);
        if (pConditional->elseBody) {
            // why are you here?
            // program->addInstruction(NOP, (uint_t) NULL, (uint_t) NULL, (uint_t) NULL);
            program->addInstruction(JMP, (uint_t) con_end_label, (uint_t) NULL,
                                    (uint_t) NULL);
        }
        program->addLabel(if_fail_label);
        if (pConditional->elseBody) {
            compileBody(pConditional->elseBody);
        }
        program->addLabel(con_end_label);
    }

    void compileVar(Var *pVar) {
        uint_t reg = (uint_t) getRegister(pVar->identifier);
        uint_t valueReg = compileExpression(pVar->value, reg);
        if(pVar->isSynchronized){
            uint_t tempReg = getRegister(NULL);
            program->addInstruction(MOV_STR,tempReg,(uint_t)pVar->identifier,0);
            program->addInstruction(SET_FIELD,1,tempReg,valueReg);
            freeRegister(tempReg);
            return;
        }
        if (valueReg - reg) {
            program->addInstruction(MOV, reg, valueReg, (uint_t) NULL);
            freeRegister(valueReg);
        }
    }

    void compileTryCatch(TryCatch *pCatch) {
        // push this finally block so that it appears before return
        if (pCatch->finallyBody)
            arraylist_push(finallyBlocksToPrependReturn, pCatch);
        char *catchLabel = (char *) malloc(100);
        char *finallyLabel = (char *) malloc(100);
        snprintf(catchLabel, 100, ".catch_%d", ++labelCount);
        snprintf(finallyLabel, 100, ".finally_%d", ++labelCount);
        program->addComment("try start");
        program->addInstruction(SET_CATCH, (uint_t) (catchLabel), (uint_t) finallyLabel, 0);
        compileBody(pCatch->tryBody);
        program->addComment("end try body");
        program->addInstruction(JMP, (uint_t) finallyLabel, 0, 0);
        program->addLabel(catchLabel);
        uint_t exceptionInfoRegister = (uint_t) getRegister(pCatch->catchIdent->data);
        program->addInstruction(POP, exceptionInfoRegister, 0, 0);
        compileBody(pCatch->catchBody);
        program->addLabel(finallyLabel);
        if (pCatch->finallyBody)
            compileBody(pCatch->finallyBody);
        program->addInstruction(CLEAR_CATCH, 0, 0, 0);
        if (pCatch->finallyBody)
            arraylist_pop(finallyBlocksToPrependReturn);
    }

    void compileStmt(Statement *statement) {
        if (statement->hasBreak) {
            program->addComment("break ");
            char *lend_label = loopEndLabelsStack->top();
            program->addInstruction(JMP, (uint_t) lend_label,
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
        } else if (stmt->kind == AST::AST_KIND_TRY_CATCH) {
            compileTryCatch(dynamic_cast<TryCatch *>(stmt));
        } else if (dynamic_cast<Expression *>(stmt)) {
            uint_t reg = compileExpression(dynamic_cast<Expression *>(stmt));
            if (statement->hasReturn) {
                //this loop compiler every finally block in the lexical scope before return statement appears.
                program->addComment(" prepend finally blocks", reg);
                for (int_t i = 0; i < finallyBlocksToPrependReturn->size; i++) {
                    compileBody(((TryCatch *) arraylist_get(finallyBlocksToPrependReturn, i))->finallyBody);
                }
                program->addComment(" return %d", reg);
                program->addInstruction(RETURN, reg, (uint_t) NULL, (uint_t) NULL);
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
                program->addComment("mov function ptr to register for %s", func->identifier);
                program->addInstruction(MOV_FNC, (uint_t) (getRegister(func->identifier)),
                                        (uint_t) func->identifier, static_cast<uint_t>(func->isAsync));
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
        char* endLabel = static_cast<char *>(malloc(100));
        snprintf(endLabel,100,"end%s",cls->identifier);
        program->addLabel("%s", cls->identifier);
        uint_t index = program->addInstruction(FFRAME, NULL, NULL, NULL);
        program->addInstruction(CREATE_THIS, (uint_t) NULL, (uint_t) NULL,
                                (uint_t) NULL);
        functionsStack->push(cls);
        for (int i = 0; i < (cls->arguments->identifiers->size()); i++) {
            char *ident = cls->arguments->identifiers->at(i)->data;
            uint_t identReg = (uint_t) (cls->getRegister(ident));
            program->addComment("pop arg %i (%s)", i, ident);
            program->addInstruction(POP, identReg, (uint_t) NULL,
                                    (uint_t) NULL);
        }
        compileBody(cls->body);
        program->addInstruction(RETURN, (uint_t) NULL, (uint_t) NULL, (uint_t) NULL);
        program->addLabel("end%s", cls->identifier);
        compileRemainingFunctions();
        uint_t locals_count = cls->registerTable->size() + cls->symbolTable->size();
        z_instruction_t *fframe = (z_instruction_t *) arraylist_get(program->instructions, index);
        //set args of FFRAME
        fframe->r0 = locals_count;
        //this will later be used to assemble required information
        fframe->r1 = (uint_t) (cls);
        //this will later be used to assemble required information
        fframe->r2 = (uint_t) (endLabel);
        functionsStack->pop();
    }

};