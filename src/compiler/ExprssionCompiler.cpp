//
// Created by onur on 27.05.2018.
//

class ExpressionCompiler {

    Program *program;

    FunctionKind *function = NULL;

    uint_t getRegister(char *ident) {
        function->getRegister(ident);
    }

    void freeRegister(uint_t index) {
        function->freeRegister(index);
    }

    uint_t toOpcode(char *operation) {
        if (strcmp(operation, ".") == 0) return GET_FIELD;
        if (strcmp(operation, "+") == 0) return ADD;
        if (strcmp(operation, "-") == 0) return SUB;
        if (strcmp(operation, "*") == 0) return MUL;
        if (strcmp(operation, "/") == 0) return DIV;
        if (strcmp(operation, "<") == 0) return CMP_LESS;
        if (strcmp(operation, "<=") == 0) return CMP_LESS_OR_EQUAL;
        if (strcmp(operation, ">") == 0) return CMP_GREATER;
        if (strcmp(operation, ">=") == 0) return CMP_GREATER_OR_EQUAL;
        if (strcmp(operation, "++") == 0) return INC;
        if (strcmp(operation, "--") == 0) return SUB;
        if (strcmp(operation, "!=") == 0) return CMP_N_EQUAL;
        if (strcmp(operation, "==") == 0) return CMP_EQUAL;
        if (strcmp(operation, "%") == 0) return MOD;
    }

public:
    uint_t compilePostfix(PostfixExpression *pExpression, int_t requestedDestinationRegister = -1) {
        uint_t leftReg = (uint_t) compileExpression(pExpression->expr);
        uint_t target = requestedDestinationRegister == -1 ? getRegister(NULL) : requestedDestinationRegister;
        program->addInstruction(MOV, target, leftReg, NULL);
        if (strcmp(pExpression->op, "++") == 0) {
            program->addInstruction(INC, leftReg, leftReg, NULL);
        } else if (strcmp(pExpression->op, "--") == 0) {
            program->addInstruction(DEC, leftReg, leftReg, NULL);
        } else {
            uint_t rightReg = (uint_t) compileTerminal(TerminalExpression::number("1"));
            program->addInstruction(toOpcode(pExpression->op), leftReg, rightReg, leftReg);
            freeRegister(rightReg);
        }
        return target;
    }

    uint_t compilePrefix(PrefixExpression *pExpression, int_t requestedDestinationRegister = -1) {
        uint_t leftReg = (uint_t) compileExpression(pExpression->expr);
        uint_t target = leftReg;
        if (strcmp(pExpression->op, "++") == 0) {
            target = (uint_t) requestedDestinationRegister == -1 ? leftReg : requestedDestinationRegister;
            program->addInstruction(INC, leftReg, target, NULL);
        } else if (strcmp(pExpression->op, "--") == 0) {
            target = (uint_t) requestedDestinationRegister == -1 ? leftReg : requestedDestinationRegister;
            program->addInstruction(DEC, leftReg, target, NULL);
        } else if (strcmp(pExpression->op, "-") == 0) {
            uint_t rightReg = (uint_t) compileTerminal(TerminalExpression::number("-1"));
            target = (uint_t) requestedDestinationRegister == -1 ? leftReg : requestedDestinationRegister;
            program->addInstruction(MUL, leftReg, rightReg, target);
            freeRegister(rightReg);
        }
        return target;
    }

    void compileDotForSetField(BinaryExpression *pExpression, int_t result) {
        int_t leftReg = compileExpression(pExpression->left);
        int_t rightReg = compileExpression(pExpression->right);
        program->addInstruction(SET_FIELD, leftReg, rightReg, result);
    }

    uint_t compileMethodCall(MethodCall *pMethodCall, int_t destinationRegister = -1) {
        ExpressionList *argumentsList = pMethodCall->argumentsList;
        vector<Expression *> *expressions = argumentsList->expressions;
        int_t resultReg = destinationRegister;
        if (resultReg == -1) resultReg = getRegister(NULL);
        uint_t exprResultReg = getRegister(NULL);
        for (int_t i = 0; i < expressions->size(); i++) {
            exprResultReg = compileExpression(expressions->at(expressions->size() - 1 - i), exprResultReg);
            // program->addComment("push arg number %i for %s ", i, pMethodCall->identifier);
            program->addInstruction(PUSH, (uint_t) (exprResultReg), (uint_t) NULL,
                                    (uint_t) NULL);
        }
        freeRegister(exprResultReg);
        exprResultReg = compileExpression(pMethodCall->callee);//compileIdentifier(pMethodCall->identifier);
        program->addInstruction(CALL,
                                (uint_t) exprResultReg, (uint_t) resultReg,
                                (uint_t) NULL);
        return resultReg;
    }

    int_t compileIdentifier(char *identifier, int_t destinationRegister = -1) {
        int_t tempRegister = getRegister(identifier);
        //value not found in the symbol table, virtual lookup
        if (tempRegister == 0) {
            if (destinationRegister == -1)
                tempRegister = getRegister(NULL);
            else tempRegister = destinationRegister;
            TerminalExpression *temp = new TerminalExpression();
            temp->type = TerminalExpression::TYPE_STRING;
            temp->data = identifier;
            int_t str_reg = compileTerminal(temp);
            program->addInstruction(GET_FIELD, 0, (uint_t) (str_reg),
                                    (uint_t) (tempRegister));
        }
        return tempRegister;
    }

    int_t compileIdentifierImmediate(char *identifier, int_t destinationRegister = -1) {
        int_t tempRegister = getRegister(identifier);
        //value not found in the symbol table, virtual lookup
        if (tempRegister == 0) {
            if (destinationRegister == -1)
                tempRegister = getRegister(NULL);
            else tempRegister = destinationRegister;
            program->addInstruction(GET_FIELD_IMMEDIATE, 0, reinterpret_cast<uint_t>(identifier),
                                    (uint_t) (tempRegister));
        }
        return tempRegister;
    }

    uint_t compileBinary(BinaryExpression *pExpression, int_t destinationRegister = -1) {

        uint_t leftReg = 0;
        uint_t rightReg = 0;
        uint_t resultReg = 0;
        int_t isSetField = 0;

        int_t isAssignment = strcmp(pExpression->op, "=") == 0;
        int_t isDot = strcmp(pExpression->op, ".") == 0;
        if (isAssignment && pExpression->left->kind == AST::AST_KIND_BINARY) {
            BinaryExpression *leftBinaryExpr = dynamic_cast<BinaryExpression *>(pExpression->left);
            if (strcmp(leftBinaryExpr->op, ".") == 0) {
                rightReg = compileExpression(pExpression->right);
                compileDotForSetField(leftBinaryExpr, rightReg);
                return rightReg;
            } else {
                error_and_exit(const_cast<char *>("left value expected"));
            }
        }
        // if we are setting a variable from a parent scope...
        if (isAssignment && pExpression->left->kind == AST::AST_KIND_TERMINAL) {
            TerminalExpression *leftValue = (TerminalExpression *) (pExpression->left);
            if (leftValue->type == TerminalExpression::TYPE_IDENTIFIER) {
                char *ident = leftValue->data;
                int_t is_left_variable_on_scope = getRegister(ident) != NULL;
                if (!is_left_variable_on_scope) {
                    leftReg = getRegister(NULL);
                    program->addInstruction(MOV_STR, (uint_t) leftReg,
                                            (uint_t) (ident), (uint_t) 0);
                    rightReg = compileExpression(pExpression->right);
                    //setfield this, $leftReg, $resutReg
                    program->addInstruction(SET_FIELD, (uint_t) (1),
                                            (uint_t) (leftReg), (uint_t) (rightReg));
                    //result of this operation is null
                    return (uint_t)0;
                }
            } else {
                error_and_exit(const_cast<char *>("left value expected"));
            }
        }

        leftReg = compileExpression(pExpression->left);
        int_t requestedDestinationRegister = -1;
        if (isAssignment) {
            requestedDestinationRegister = leftReg;
        }
        if (isDot) rightReg = compileExpression(pExpression->right, requestedDestinationRegister, 1);
        else
            rightReg = compileExpression(pExpression->right, requestedDestinationRegister);
        if (destinationRegister != -1) {
            resultReg = destinationRegister;
        } else {
            resultReg = getRegister(NULL);
        }
        if (!isAssignment) {
            program->addInstruction(toOpcode(pExpression->op), (uint_t) (leftReg),
                                    (uint_t) (rightReg), (uint_t) (resultReg));
        }
        if (rightReg != resultReg)
            freeRegister(rightReg);
        freeRegister(leftReg);

        return resultReg;
    }

    int_t compileTerminal(TerminalExpression *right, int_t destionationRegister = -1, int_t has_lookup_object = 0) {
        if (right->type == TerminalExpression::TYPE_IDENTIFIER) {
            if (has_lookup_object) compileIdentifier(right->data);
            return compileIdentifierImmediate(right->data);
        }
        int_t reg = destionationRegister;
        if (reg == -1) reg = getRegister(NULL);
        if (right->type == TerminalExpression::TYPE_NUMBER) {
            program->addInstruction(MOV_NUMBER, (uint_t) (reg), (uint_t) right->data,
                                    (uint_t) NULL);
            return reg;
        }
        if (right->type == TerminalExpression::TYPE_STRING) {
            program->addInstruction(MOV_STR, (uint_t) (reg), (uint_t) right->data,
                                    (uint_t) NULL);
            return reg;
        }
    }

    uint_t compileExpression(Expression *expr, int_t requestedDestinationRegister = -1, int_t has_lookup_object = 0) {
        int_t reg;
        if (expr->kind == AST::AST_KIND_METHOD_CALL) {
            return compileMethodCall(dynamic_cast<MethodCall *>(expr),
                                     requestedDestinationRegister);
        } else if (expr->kind == AST::AST_KIND_BINARY) {
            return compileBinary(dynamic_cast<BinaryExpression *>(expr),
                                 requestedDestinationRegister);
        } else if (expr->kind == AST::AST_KIND_POSTFIX) {
            return compilePostfix(dynamic_cast<PostfixExpression *>(expr),
                                  requestedDestinationRegister);
        } else if (expr->kind == AST::AST_KIND_PREFIX) {
            return compilePrefix(dynamic_cast<PrefixExpression *>(expr),
                                 requestedDestinationRegister);
        } else if (expr->kind == AST::AST_KIND_TERMINAL) {
            return compileTerminal(dynamic_cast<TerminalExpression *>(expr),
                                   requestedDestinationRegister, has_lookup_object);
        }
    }

    ExpressionCompiler(Program *program, FunctionKind *function) {
        this->program = program;
        this->function = function;
    }
};