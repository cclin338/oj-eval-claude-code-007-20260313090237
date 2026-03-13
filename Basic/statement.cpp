/*
 * File: statement.cpp
 * -------------------
 * This file implements the Statement class and its subclasses.
 */

#include "statement.hpp"
#include <iostream>

Statement::Statement() = default;

Statement::~Statement() = default;

// REM statement
RemStatement::RemStatement(TokenScanner &scanner) {
    // REM ignores everything after it
}

RemStatement::~RemStatement() {
}

void RemStatement::execute(EvalState &state, Program &program) {
    // Do nothing
}

// LET statement
LetStatement::LetStatement(TokenScanner &scanner) {
    exp = parseExp(scanner);
}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program) {
    exp->eval(state);
}

// PRINT statement
PrintStatement::PrintStatement(TokenScanner &scanner) {
    exp = parseExp(scanner);
}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    std::cout << value << std::endl;
}

// INPUT statement
InputStatement::InputStatement(TokenScanner &scanner) {
    std::string token = scanner.nextToken();
    if (scanner.getTokenType(token) != WORD) {
        error("SYNTAX ERROR");
    }
    var = token;
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

InputStatement::~InputStatement() {
}

void InputStatement::execute(EvalState &state, Program &program) {
    std::cout << " ? ";
    std::string line;
    while (true) {
        if (!getline(std::cin, line)) {
            // EOF reached
            exit(0);
        }
        TokenScanner scanner;
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        scanner.setInput(line);
        std::string token = scanner.nextToken();
        if (scanner.getTokenType(token) == NUMBER && !scanner.hasMoreTokens()) {
            state.setValue(var, stringToInteger(token));
            break;
        } else {
            std::cout << "INVALID NUMBER" << std::endl;
            std::cout << " ? ";
        }
    }
}

// END statement
EndStatement::EndStatement(TokenScanner &scanner) {
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

EndStatement::~EndStatement() {
}

void EndStatement::execute(EvalState &state, Program &program) {
    // Signal to stop execution by throwing an exception or setting a flag
    // We'll use an exception for simplicity
    throw std::runtime_error("END");
}

// GOTO statement
GotoStatement::GotoStatement(TokenScanner &scanner) {
    std::string token = scanner.nextToken();
    if (scanner.getTokenType(token) != NUMBER) {
        error("SYNTAX ERROR");
    }
    targetLine = stringToInteger(token);
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

GotoStatement::~GotoStatement() {
}

void GotoStatement::execute(EvalState &state, Program &program) {
    // Signal to jump to target line by throwing an exception with line number
    throw targetLine;
}

// IF statement
IfStatement::IfStatement(TokenScanner &scanner) {
    // Read LHS expression, stopping before comparison operators (precedence 2)
    lhs = readE(scanner, 1);

    std::string oper = scanner.nextToken();
    if (oper != "=" && oper != "<" && oper != ">") {
        error("SYNTAX ERROR");
    }
    op = oper;

   // Read RHS expression, stopping before THEN
    rhs = readE(scanner, 1);

    std::string then = scanner.nextToken();
    if (then != "THEN") {
        error("SYNTAX ERROR");
    }

    std::string lineStr = scanner.nextToken();
    if (scanner.getTokenType(lineStr) != NUMBER) {
        error("SYNTAX ERROR");
    }
    targetLine = stringToInteger(lineStr);

    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

IfStatement::~IfStatement() {
    delete lhs;
    delete rhs;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int left = lhs->eval(state);
    int right = rhs->eval(state);

    bool condition = false;
    if (op == "=") {
        condition = (left == right);
    } else if (op == "<") {
        condition = (left < right);
    } else if (op == ">") {
        condition = (left > right);
    }

    if (condition) {
        throw targetLine; // Jump to target line
    }
}
