/*
 * File: program.cpp
 * -----------------
 * This file implements the program.h interface.
 */

#include "program.hpp"

Program::Program() {
}

Program::~Program() {
    clear();
}

void Program::clear() {
    // Delete all parsed statements
    for (auto &pair : parsedStatements) {
        delete pair.second;
    }
    parsedStatements.clear();
    sourceLines.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    // If line already exists, remove the old parsed statement
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
    sourceLines[lineNumber] = line;
}

void Program::removeSourceLine(int lineNumber) {
    // Remove the source line
    sourceLines.erase(lineNumber);
    // Remove and delete the parsed statement
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    if (sourceLines.find(lineNumber) != sourceLines.end()) {
        return sourceLines[lineNumber];
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // If there's already a parsed statement, delete it
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
    }
    parsedStatements[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        return parsedStatements[lineNumber];
    }
    return nullptr;
}

int Program::getFirstLineNumber() {
    if (sourceLines.empty()) {
        return -1;
    }
    return sourceLines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it != sourceLines.end()) {
        ++it;
        if (it != sourceLines.end()) {
            return it->first;
        }
    }
    return -1;
}
