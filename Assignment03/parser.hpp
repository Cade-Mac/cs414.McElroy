//
// File:   parser.cpp
// Author: Your Glorious Instructor
// Purpose:
// Parse a TinyML program using a recursive descent parser
//

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <stdexcept>
#include "tokenizer.hpp"

class Parser {
public:
    explicit Parser(Tokenizer& tokenizer) : tokenizer(tokenizer) {}

    void parse() {
        parseStmtList();
    }

private:
    Tokenizer& tokenizer;
    std::unordered_map<std::string, int> symbolTable;

    void expect(TokenType type) {
        if (tokenizer.currentToken().type != type) {
            throw std::runtime_error("Syntax error: unexpected token");
        }
        tokenizer.nextToken();
    }

    void parseStmtList() {
        while (tokenizer.currentToken().type != TokenType::END) {
            parseStmt();
        }
    }

    void parseStmt() {
        if (tokenizer.currentToken().type == TokenType::IDENTIFIER) {
             parseAssign();
        } else if (tokenizer.currentToken().type == TokenType::PRINT) {
            parsePrint();
        } else {
            throw std::runtime_error("Syntax error: invalid statement");
        }
        expect(TokenType::SEMICOLON);
    }

    void parseAssign() {
        std::string identifier = tokenizer.currentToken().value;
        expect(TokenType::IDENTIFIER);
        expect(TokenType::ASSIGN);
        int value = parseExpr();
        symbolTable[identifier] = value;
    }

    void parsePrint() {
        expect(TokenType::PRINT);
        expect(TokenType::LPAREN);
        std::string identifier = tokenizer.currentToken().value;
        expect(TokenType::IDENTIFIER);
        expect(TokenType::RPAREN);
        if (symbolTable.find(identifier) != symbolTable.end()) {
            std::cout << identifier << " = " << symbolTable[identifier] << std::endl;
        } else {
            throw std::runtime_error("Undefined variable: " + identifier);
        }
    }

    int parseExpr() {
        int value = parseTerm();
        while (tokenizer.currentToken().type == TokenType::PLUS ||
               tokenizer.currentToken().type == TokenType::MINUS) {
            TokenType op = tokenizer.currentToken().type;
            tokenizer.nextToken();
            int rhs = parseTerm();
            if (op == TokenType::PLUS) value += rhs;
            else value -= rhs;
        }
        return value;
    }

    int parseTerm() {
        int value = parseFactor();
        while (tokenizer.currentToken().type == TokenType::MUL ||
               tokenizer.currentToken().type == TokenType::DIV) {
            TokenType op = tokenizer.currentToken().type;
            tokenizer.nextToken();
            int rhs = parseFactor();
            if (op == TokenType::MUL) value *= rhs;
            else value /= rhs;
        }
        return value;
    }

    int parseFactor() {
        if (tokenizer.currentToken().type == TokenType::NUMBER) {
            int value = std::stoi(tokenizer.currentToken().value);
            tokenizer.nextToken();
            return value;
        } else if (tokenizer.currentToken().type == TokenType::IDENTIFIER) {
            std::string identifier = tokenizer.currentToken().value;
            tokenizer.nextToken();
            if (symbolTable.find(identifier) != symbolTable.end()) {
                return symbolTable[identifier];
            } else {
                throw std::runtime_error("Undefined variable: " + identifier);
            }
        } else if (tokenizer.currentToken().type == TokenType::LPAREN) {
            tokenizer.nextToken();
            int value = parseExpr();
            expect(TokenType::RPAREN);
            return value;
        } else {
            throw std::runtime_error("Syntax error: invalid factor");
        }
    }
};