//
// Created by onur on 26.05.2018.
//

class AddressCalculator {

private:
    stack<FunctionKind *> *functionsStack = new stack<FunctionKind *>();

    map<string, int> *getCurrentSymbolTable() {
        return functionsStack->top()->symbolTable;
    };

    void addToCurrentTable(char *identifier) {
        int count = getCurrentSymbolTable()->size() + 1;
        getCurrentSymbolTable()->insert(pair<string, int>(identifier, count));
    }

public:
    AddressCalculator(AST *ast) {
        calculateClass(dynamic_cast<ClassDeclaration *>(ast));
    }

    void calculateFunction(Function *func) {
        addToCurrentTable(func->identifier);
        functionsStack->push(func);
        addToCurrentTable(const_cast<char *>("this"));
        addToCurrentTable(func->identifier);
        for (int i = 0; i < func->arguments->identifiers->size(); i++) {
            addToCurrentTable(func->arguments->identifiers->at(i)->data);
        }
        calculateBody(func->body);
        functionsStack->pop();
    }

    void calculateLoop(Loop *pLoop) {
        calculateBody(pLoop->body);
    }

    void calculateConditional(Conditional *pCond) {
        calculateBody(pCond->body);
        if (pCond->elseBody)
            calculateBody(pCond->elseBody);
    }

    void calculateStatement(Statement *statement) {
        AST *stmt = statement->stmt;
        if (stmt->kind == AST::AST_KIND_FUNCTION) {
            calculateFunction(dynamic_cast<Function *>(stmt));
        }
        if (stmt->kind == AST::AST_KIND_LOOP) {
            calculateLoop(dynamic_cast<Loop *>(stmt));
        }
        if (stmt->kind == AST::AST_KIND_BODY) {
            calculateBody(dynamic_cast<Body *>(stmt));
        }
        if (stmt->kind == AST::AST_KIND_CONDITIONAL) {
            calculateConditional(dynamic_cast<Conditional *>(stmt));
        }
        if (stmt->kind == AST::AST_KIND_VAR) {
            calculateVar(dynamic_cast<Var *>(stmt));
        }
    }

    void calculateBody(Body *pBody) {
        vector<Statement *> *statements = pBody->getStatements();
        for (int i = 0; i < statements->size(); i++) {
            Statement *stmt = statements->at(i);
            calculateStatement(stmt);
        }
    }

    void calculateClass(ClassDeclaration *cls) {
        functionsStack->push(cls);
        addToCurrentTable(const_cast<char *>("this"));
        calculateBody(cls->body);
    }

    void calculateVar(Var *var) {
        addToCurrentTable(var->identifier);
    }

};

