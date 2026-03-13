/*
 * File: Basic.cpp
 * ---------------
 * This file is the main program for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"
#include "statement.hpp"

/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    while (true) {
        try {
            std::string input;
            if (!getline(std::cin, input)) {
                break; // EOF reached
            }
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    std::string token = scanner.nextToken();
    TokenType type = scanner.getTokenType(token);

    // Check if it's a line number
    if (type == NUMBER) {
        int lineNumber = stringToInteger(token);

        // Check if there are more tokens (statement to add)
        if (scanner.hasMoreTokens()) {
            // Store the line (without the line number)
            std::string restOfLine = line.substr(token.length());
            // Trim leading whitespace
            size_t start = restOfLine.find_first_not_of(" \t");
            if (start != std::string::npos) {
                restOfLine = restOfLine.substr(start);
            }
            program.addSourceLine(lineNumber, line);

            // Parse the statement
            TokenScanner stmtScanner;
            stmtScanner.ignoreWhitespace();
            stmtScanner.scanNumbers();
            stmtScanner.setInput(restOfLine);

            std::string cmd = stmtScanner.nextToken();
            Statement *stmt = nullptr;

            try {
                if (cmd == "REM") {
                    stmt = new RemStatement(stmtScanner);
                } else if (cmd == "LET") {
                    stmt = new LetStatement(stmtScanner);
                } else if (cmd == "PRINT") {
                    stmt = new PrintStatement(stmtScanner);
                } else if (cmd == "INPUT") {
                    stmt = new InputStatement(stmtScanner);
                } else if (cmd == "END") {
                    stmt = new EndStatement(stmtScanner);
                } else if (cmd == "GOTO") {
                    stmt = new GotoStatement(stmtScanner);
                } else if (cmd == "IF") {
                    stmt = new IfStatement(stmtScanner);
                } else {
                    error("SYNTAX ERROR");
                }
                program.setParsedStatement(lineNumber, stmt);
            } catch (ErrorException &ex) {
                delete stmt;
                throw;
            }
        } else {
            // Just a line number, remove that line
            program.removeSourceLine(lineNumber);
        }
    }
    // Direct command execution
    else if (type == WORD) {
        if (token == "RUN") {
            if (scanner.hasMoreTokens()) {
                error("SYNTAX ERROR");
            }
            // Execute the program
            int currentLine = program.getFirstLineNumber();
            while (currentLine != -1) {
                try {
                    Statement *stmt = program.getParsedStatement(currentLine);
                    if (stmt != nullptr) {
                        stmt->execute(state, program);
                    }
                    currentLine = program.getNextLineNumber(currentLine);
                } catch (int targetLine) {
                    // GOTO or IF jumped to target line
                    currentLine = targetLine;
                    // Check if target line exists
                    if (program.getSourceLine(currentLine).empty()) {
                        error("LINE NUMBER ERROR");
                    }
                } catch (std::runtime_error &ex) {
                    // END statement
                    break;
                } catch (ErrorException &ex) {
                    // Error during execution, propagate it
                    throw;
                }
            }
        } else if (token == "LIST") {
            if (scanner.hasMoreTokens()) {
                error("SYNTAX ERROR");
            }
            int currentLine = program.getFirstLineNumber();
            while (currentLine != -1) {
                std::cout << program.getSourceLine(currentLine) << std::endl;
                currentLine = program.getNextLineNumber(currentLine);
            }
        } else if (token == "CLEAR") {
            if (scanner.hasMoreTokens()) {
                error("SYNTAX ERROR");
            }
            program.clear();
            state.Clear();
        } else if (token == "QUIT") {
            if (scanner.hasMoreTokens()) {
                error("SYNTAX ERROR");
            }
            exit(0);
        } else if (token == "HELP") {
            // Optional, not tested
            std::cout << "BASIC Interpreter Help" << std::endl;
        } else if (token == "LET") {
            // Direct LET execution
            LetStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (token == "PRINT") {
            // Direct PRINT execution
            PrintStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (token == "INPUT") {
            // Direct INPUT execution
            InputStatement stmt(scanner);
            stmt.execute(state, program);
        } else {
            error("SYNTAX ERROR");
        }
    } else {
        error("SYNTAX ERROR");
    }
}
