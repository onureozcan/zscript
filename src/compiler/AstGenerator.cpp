//
// Created by onur on 12.07.2018.
//

#include "compiler.h"

class AstGenerator {

public:
    ClassDeclaration *cls = NULL;
    Function *staticConstructor = new Function();


    AstGenerator(zeroscriptParser::ClassDeclarationContext *classDeclaration) {
        staticConstructor->setIdentifier("__static__constructor__");
        staticConstructor->body = new Body();
        staticConstructor->arguments = new ArgumentList();
        visitClassDeclaration(classDeclaration);
        Statement *stmt = new Statement();
        this->cls->body->statements->push_back(stmt);
        stmt->stmt = staticConstructor;
    }

    void addStaticVar(Var* var){
        Statement* stmt = new Statement();
        //static[ident] = value
        char* ident = var->identifier;
        BinaryExpression* bop = new BinaryExpression();
        bop->setOp(".");
        bop->left = TerminalExpression::identifier("__static__");
        bop->right = TerminalExpression::stringWithoutTrim(ident);

        BinaryExpression* assign = new BinaryExpression();
        assign->setOp("=");
        assign->left = bop;
        assign->right = var->value;

        stmt->stmt = assign;
        staticConstructor->body->statements->push_back(stmt);
    }

    Var *visitVarDeclaration(zeroscriptParser::VariableDeclarationPartContext *part, Body *pKind, bool isStatic) {
        Var *var = new Var();
        var->setIdentifier(part->variableName->IDENT()->getText().data());
        var->isStatic = isStatic;
        if (part->expression()) {
            var->value = visitExpression(part->expression(), pKind);
        } else var->value = new EmptyExpression();
        return var;
    }

    vector<Var *> *visitVar(zeroscriptParser::VarContext *varcontext, Body *pKind) {
        vector<Var *> *vars = new vector<Var *>();
        vector<zeroscriptParser::VariableDeclarationPartContext *> declarations = varcontext->variableDeclarationPart();
        if (varcontext->STATIC() != NULL) {
            for (int i = 0; i < declarations.size(); i++) {
                addStaticVar(visitVarDeclaration(declarations.at(i), pKind, varcontext->STATIC() != NULL));
            }
        } else {
            for (int i = 0; i < declarations.size(); i++) {
                vars->push_back(visitVarDeclaration(declarations.at(i), pKind, varcontext->STATIC() != NULL));
            }
        }
        return vars;
    }

    TerminalExpression *visitIdent(zeroscriptParser::IdentifierContext *context, Body *pKind) {
        return TerminalExpression::identifier(context->IDENT()->getText().data());
    }

    ArgumentList *visitArguments(zeroscriptParser::ArgumentsListContext *pContext, Body *pKind) {
        ArgumentList *args = new ArgumentList();
        vector<zeroscriptParser::IdentifierContext *> terminals = pContext->identifier();
        for (int i = 0; i < terminals.size(); i++) {
            args->addIdent(visitIdent(terminals.at(i), pKind));
        }
        return args;
    }

    Function *visitFunction(zeroscriptParser::FunctionContext *pContext, Body *pKind) {
        Function *func = new Function();
        func->setIdentifier(pContext->functionName->IDENT()->getText().data());
        func->body = visitBody(pContext->body(), pKind);
        func->arguments = visitArguments(pContext->argumentsList(), pKind);
        return func;
    }

    Expression *visitAtom(zeroscriptParser::AtomContext *pContext, Body *parentContext) {
        if (pContext->identifier())
            return TerminalExpression::identifier(pContext->identifier()->IDENT()->getText().data());
        if (pContext->string()) return TerminalExpression::string(pContext->string()->STRING()->getText().data());
        if (pContext->number()) {
            if (pContext->number()->DECIMAL()) {
                return TerminalExpression::number(pContext->number()->DECIMAL()->getText().data());
            } else if (pContext->number()->INT()) {
                return TerminalExpression::number(pContext->number()->INT()->getText().data());
            } else if (pContext->number()->FALSE_()) {
                return TerminalExpression::number("0");
            } else if (pContext->number()->TRUE_()) {
                return TerminalExpression::number("1");
            }
        }
        if (pContext->json()) {
            //TODO
            zeroscriptParser::JsonContext *jsonObj = pContext->json();
            return visitJson(jsonObj, parentContext);
        }
        return nullptr;
    }

    ExpressionList *visitExpressionList(zeroscriptParser::ExpressionListContext *pContext, Body *pKind) {
        ExpressionList *expressions = new ExpressionList();
        if (pContext != NULL)
            for (auto &expr : pContext->expression()) {
                expressions->addExpression(visitExpression(expr, pKind));
            }
        return expressions;
    }

    int counter = 0;

    Expression *visitAnonymousFunction(zeroscriptParser::AnonymousFunctionContext *pContext, Body *pKind) {
        std::string name = "$anonymous_" + std::to_string(++counter);
        Function *func = new Function();
        func->setIdentifier(name.data());
        if (pContext->body()) {
            func->body = visitBody(pContext->body(), pKind);
        } else if (pContext->bodyOrExpression()->body()) {
            func->body = visitBody(pContext->bodyOrExpression()->body(), pKind);
        }
        if (pContext->argumentsList()) {
            func->arguments = visitArguments(pContext->argumentsList(), pKind);
        } else {
            func->arguments = new ArgumentList();
            func->arguments->addIdent(
                    TerminalExpression::identifier(pContext->identifier()->IDENT()->getText().data()));
        }
        Statement *stmt = new Statement();
        stmt->stmt = func;
        pKind->statements->push_back(stmt);
        return TerminalExpression::identifier(name.data());
    }

    Expression *visitExpression(zeroscriptParser::ExpressionContext *pContext, Body *pKind) {
        if (pContext->primaryExpresssion()) {
            return visitPrimaryExpression(pContext->primaryExpresssion(), pKind);
        } else if (pContext->bop) {
            Expression *left = visitExpression(pContext->expression(0), pKind);
            Expression *right = visitExpression(pContext->expression(1), pKind);
            BinaryExpression *expr = new BinaryExpression();
            expr->left = left;
            expr->right = right;
            expr->setOp(pContext->bop->getText().data());
            if (strcmp(expr->op, ".") == 0) {
                if (expr->right->kind == AST::AST_KIND_TERMINAL) {
                    TerminalExpression *ident = dynamic_cast<TerminalExpression *>(expr->right);
                    if (ident->type == TerminalExpression::TYPE_IDENTIFIER) {
                        ident->type = TerminalExpression::TYPE_STRING;
                    }
                }
            }
            return expr;
        } else if (pContext->prefix) {
            Expression *right = visitExpression(pContext->expression(0), pKind);
            PrefixExpression *expr = new PrefixExpression();
            expr->expr = right;
            expr->setOp(pContext->prefix->getText().data());
            return expr;
        } else if (pContext->postfix) {
            Expression *left = visitExpression(pContext->expression(0), pKind);
            PostfixExpression *expr = new PostfixExpression();
            expr->expr = left;
            expr->setOp(pContext->postfix->getText().data());
            return expr;
        } else if (pContext->methodCall) {
            MethodCall *methodCall = visitMethodCall(pContext, pKind);
            return methodCall;
        } else if (pContext->anonymousFunction()) {
            return visitAnonymousFunction(pContext->anonymousFunction(), pKind);
        } else if (pContext->arrayIndexer) {
            Expression *left = visitExpression(pContext->expression(0), pKind);
            Expression *right = visitExpression(pContext->expression(1), pKind);
            BinaryExpression *expr = new BinaryExpression();
            expr->left = left;
            expr->right = right;
            expr->setOp(".");
            return expr;
        } else if (pContext->newObject()) {
            return visitMethodCall(pContext->newObject()->expression(), pKind);
        }
        return nullptr;
    }

    MethodCall *visitMethodCall(zeroscriptParser::ExpressionContext *pContext, Body *pKind) {
        MethodCall *methodCall = new MethodCall();
        Expression *callee = visitExpression(pContext->expression(0), pKind);
        methodCall->callee = callee;
        methodCall->argumentsList = visitExpressionList(pContext->expressionList(), pKind);
        return methodCall;
    }

    Expression *visitPrimaryExpression(zeroscriptParser::PrimaryExpresssionContext *pContext, Body *pKind) {
        if (pContext->expression()) {
            return visitExpression(pContext->expression(), pKind);
        } else {
            return visitAtom(pContext->atom(), pKind);
        }
    }

    int forIndexerCount = 0;

    //for in loop is a syntactic sugar
    //we convert it into a for loop
    Loop *visitForInLoop(zeroscriptParser::ForInLoopContext *pContext, Body *parentBody) {
        Loop *loop = new Loop();
        Var *iterItem = new Var();
        iterItem->setIdentifier(pContext->iterElement->getText().data());
        iterItem->value = TerminalExpression::number("0");
        Statement *stmt = new Statement();
        stmt->stmt = iterItem;
        parentBody->statements->push_back(stmt);
        Var *indexer = new Var();
        indexer->value = TerminalExpression::number("0");
        char indexerName[100];
        snprintf(indexerName, 99, "$indexer%d", forIndexerCount++);
        indexer->setIdentifier(indexerName);
        Statement *stmt2 = new Statement();
        stmt2->stmt = indexer;
        parentBody->statements->push_back(stmt2);
        //objectToIterate = iter
        Expression *objectToIterate = visitExpression(pContext->expression(), parentBody);
        MethodCall *objectKeysCall = new MethodCall();
        objectKeysCall->argumentsList = new ExpressionList();
        //objectKeys = iter.keys
        BinaryExpression *objectKeys = new BinaryExpression();
        objectKeys->left = objectToIterate;
        objectKeys->right = TerminalExpression::string(" keys ");
        objectKeys->setOp(".");
        //objectKeysCall = iter.keys()
        objectKeysCall->callee = objectKeys;
        //objectKeysSize = iter.size
        BinaryExpression *objectKeysSize = new BinaryExpression();
        objectKeysSize->left = objectToIterate;
        objectKeysSize->right = TerminalExpression::string(" size ");
        objectKeysSize->setOp(".");
        //objectKeysSizeCall = iter.size()
        MethodCall *objectKeysSizeCall = new MethodCall();
        objectKeysSizeCall->argumentsList = new ExpressionList();
        objectKeysSizeCall->callee = objectKeysSize;

        //condition = iter.size() > i
        BinaryExpression *condition = new BinaryExpression();
        condition->left = objectKeysSizeCall;
        condition->right = TerminalExpression::identifier(indexerName);
        condition->setOp(">");

        loop->condition = condition;
        loop->startExpr = NULL;

        PostfixExpression *iter = new PostfixExpression();
        iter->expr = TerminalExpression::identifier(indexerName);
        iter->setOp("++");

        loop->iterExpr = iter;


        if (pContext->bodyOrStatement()->body()) {
            loop->body = visitBody(pContext->bodyOrStatement()->body(), parentBody);
        } else {
            loop->body = new Body();
            loop->body->statements = visitStatement(pContext->bodyOrStatement()->statement(), parentBody);
        }

        Statement *elemAccessStmt = new Statement();
        loop->body->statements->insert(loop->body->statements->begin(), elemAccessStmt);

        //elemAccessExpression => iterElement = objectKeysCall[indexer]
        BinaryExpression *elemAccessExpression = new BinaryExpression();
        elemAccessExpression->setOp("=");
        elemAccessExpression->left = TerminalExpression::identifier(pContext->iterElement->getText().data());

        //objectKeysCall[indexer]
        BinaryExpression *keyExpr = new BinaryExpression();
        keyExpr->setOp(".");
        keyExpr->left = objectKeysCall;
        keyExpr->right = TerminalExpression::identifier(indexerName);

        elemAccessExpression->right = keyExpr;

        elemAccessStmt->stmt = elemAccessExpression;
        return loop;
    }

    Loop *visitForLoop(zeroscriptParser::ForLoopContext *pContext, Body *parentBody) {
        Loop *loop = new Loop();
        size_t hasStartExpr = 0;
        if (pContext->var()) {
            vector<Var *> *variables = visitVar(pContext->var(), parentBody);
            for (int i = 0; i < variables->size(); i++) {
                Statement *stmt = new Statement();
                stmt->stmt = variables->at(i);
                stmt->line_number = static_cast<int>(pContext->var()->VAR()->getSymbol()->getLine());
                parentBody->statements->push_back(stmt);
            }
        } else if (pContext->expression().size() > 2) {
            hasStartExpr = 1;
            loop->startExpr = visitExpression(pContext->expression(0), parentBody);
        } else {
            loop->startExpr = NULL;
        }
        if (pContext->bodyOrStatement()->body()) {
            loop->body = visitBody(pContext->bodyOrStatement()->body(), parentBody);
        } else {
            loop->body = new Body();
            loop->body->statements = visitStatement(pContext->bodyOrStatement()->statement(), parentBody);
        }
        if (pContext->expression(hasStartExpr)) {
            loop->condition = visitExpression(pContext->expression(hasStartExpr), parentBody);
        }
        if (pContext->expression(hasStartExpr + 1)) {
            loop->iterExpr = visitExpression(pContext->expression(hasStartExpr + 1), parentBody);
        }
        return loop;
    }

    Loop *visitWhileLoop(zeroscriptParser::WhileLoopContext *pContext, Body *pKind) {
        return nullptr;
    }

    Conditional *visitConditional(zeroscriptParser::ConditionalContext *pContext, Body *pKind) {
        Conditional *cond = new Conditional();
        cond->condition = visitExpression(pContext->expression(), pKind);
        if (pContext->bodyOrStatement(0)->body()) {
            cond->body = visitBody(pContext->bodyOrStatement(0)->body(), pKind);
        } else {
            cond->body = new Body();
            cond->body->statements = visitStatement(pContext->bodyOrStatement(0)->statement(), pKind);
        }
        if (pContext->ELSE()) {
            if (pContext->bodyOrStatement(1)->body()) {
                cond->elseBody = visitBody(pContext->bodyOrStatement(1)->body(), pKind);
            } else {
                cond->elseBody = new Body();
                cond->elseBody->statements = visitStatement(pContext->bodyOrStatement(1)->statement(), pKind);
            }
        }
        return cond;
    }

    int jsonObjectCount = 0;

    Expression *visitJson(zeroscriptParser::JsonContext *json, Body *pBody) {

        Statement *stmt = new Statement();
        //json is a syntactic sugar.
        //we are going to build this template.
        //var $obj{jsonObjectCount} = new Object();
        MethodCall *call = new MethodCall();
        TerminalExpression *expr = TerminalExpression::identifier("Object");
        call->callee = expr;
        call->argumentsList = new ExpressionList();

        char *nameOfTheTempObject = const_cast<char *>((std::string("$tempJson") +
                                                        std::to_string(++jsonObjectCount)).data());

        Var *var = new Var();
        var->setIdentifier(std::string(nameOfTheTempObject).data());
        var->value = call;

        stmt->stmt = var;
        pBody->statements->push_back(stmt);

        if (json->jsonObject()) {
            visitJsonObject(json->jsonObject(), nameOfTheTempObject, pBody);
        } else {
            visitJsonArray(json->jsonArray(), nameOfTheTempObject, pBody);
        }

        return TerminalExpression::identifier(nameOfTheTempObject);
    }

    void visitJsonPair(zeroscriptParser::JsonPairContext *pair, char *nameOfTheTempObj, Body *pBody) {
        zeroscriptParser::ExpressionContext *keyContext = pair->key;
        Expression *key = visitExpression(keyContext, pBody);
        Expression *value = visitExpression(pair->expression(1), pBody);
        doHandleJsonPair(nameOfTheTempObj, pBody, key, value);

    }

    void doHandleJsonPair(const char *nameOfTheTempObj, const Body *pBody, Expression *key,
                          Expression *value) const {//we will build the following template
        //{nameOfTheTempObj}[key] = value;
        BinaryExpression *keyAddressOpereation = new BinaryExpression();
        keyAddressOpereation->setOp(".");
        keyAddressOpereation->left = TerminalExpression::identifier(
                nameOfTheTempObj
        );
        keyAddressOpereation->right = key;

        BinaryExpression *assignmentOperation = new BinaryExpression();
        assignmentOperation->left = keyAddressOpereation;
        assignmentOperation->right = value;
        assignmentOperation->setOp("=");

        Statement *stmt = new Statement();
        stmt->stmt = assignmentOperation;

        pBody->statements->push_back(stmt);
    }

    void visitJsonObject(zeroscriptParser::JsonObjectContext *json, char *nameOfTheTempObj, Body *pBody) {

        for (int i = 0; i < json->jsonPair().size(); i++) {
            visitJsonPair(json->jsonPair(i), nameOfTheTempObj, pBody);
        }

    }

    void visitJsonArray(zeroscriptParser::JsonArrayContext *json, char *nameOfTheTempObj, Body *pBody) {

        for (size_t i = 0; i < json->expression().size(); i++) {
            doHandleJsonPair(nameOfTheTempObj, pBody, TerminalExpression::string(
                    (std::string(" ") + std::to_string(i) + std::string(" ")).data()
            ), visitExpression(json->expression(i), pBody));
        }

    }

    vector<Statement *> *visitStatement(zeroscriptParser::StatementContext *statement, Body *pKind) {
        vector<Statement *> *resultStatements = new vector<Statement *>();
        Statement *stmt = new Statement();
        resultStatements->push_back(stmt);
        if (statement->semicolon) {
            stmt->stmt = new EmptyExpression();
            return resultStatements;
        }
        if (statement->body()) {
            stmt->stmt = visitBody(statement->body(), pKind);
        } else if (statement->var()) {
            vector<Var *> *variables = visitVar(statement->var(), pKind);
            resultStatements = new vector<Statement *>();
            for (int i = 0; i < variables->size(); i++) {
                Statement *s = new Statement();
                s->stmt = variables->at(i);
                s->line_number = statement->var()->VAR()->getSymbol()->getLine();
                resultStatements->push_back(s);
            }
        } else if (statement->function()) {
            stmt->stmt = visitFunction(statement->function(), pKind);
        } else if (statement->expression()) {
            stmt->stmt = visitExpression(statement->expression(), pKind);
        } else if (statement->forLoop()) {
            stmt->stmt = visitForLoop(statement->forLoop(), pKind);
        } else if (statement->forInLoop()) {
            stmt->stmt = visitForInLoop(statement->forInLoop(), pKind);
        } else if (statement->whileLoop()) {
            stmt->stmt = visitWhileLoop(statement->whileLoop(), pKind);
        } else if (statement->conditional()) {
            stmt->stmt = visitConditional(statement->conditional(), pKind);
        } else if (statement->tryCatch()) {
            //TODO
        } else if (statement->throw_()) {
            //TODO
        }
        if (resultStatements->size() > 0) {
            stmt = resultStatements->at(0);
            stmt->hasBreak = statement->BREAK() != NULL;
            stmt->hasContinue = statement->CONTINUE() != NULL;
            stmt->hasReturn = statement->RET() != NULL;
            if (stmt->hasBreak || stmt->hasContinue) {
                stmt->stmt = new EmptyExpression();
            }
        }
        return resultStatements;
    }

    Body *visitBody(zeroscriptParser::BodyContext *context, Body *parentBody) {
        Body *body = new Body();
        vector<zeroscriptParser::StatementContext *> statements = context->statement();
        for (int i = 0; i < statements.size(); i++) {
            vector<Statement *> *resultStatements = visitStatement(statements.at(i), body);
            for (int j = 0; j < resultStatements->size(); j++) {
                body->statements->push_back(resultStatements->at(j));
            }
        }
        return body;
    }

    void visitClassDeclaration(zeroscriptParser::ClassDeclarationContext *context) {
        cls = new ClassDeclaration();
        cls->body = visitBody(context->body(), NULL);
        cls->setIdentifier(context->identifier().at(0)->getText().data());
    }


    ClassDeclaration *getRootClass() {
        return cls;
    }

};
