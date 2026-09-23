/*
 * CS 414 
 * Assignment 04
 * Extended Shell Command Parser with Variables, Expression Parsing, and Symbol Table
 * Cade McElroy
 * 
 * =========================================================================================
 * EBNF Grammar Specification for Extended Shell Commands:
 * 
 * Start Symbol: Command
 * 
 * Command     ::= LsCmd | CdCmd | CatCmd | PrintCmd | ExecCmd | SetCmd | EchoCmd ;
 * 
 * SetCmd      ::= "set" VARIABLE "=" Expr ;
 * EchoCmd     ::= "echo" ( VARIABLE | Expr ) ;
 * 
 * LsCmd       ::= "ls" [ Path | Filename | VARIABLE ] ;
 * CdCmd       ::= "cd" [ Path | VARIABLE ] ;
 * CatCmd      ::= "cat" ( Path | Filename | VARIABLE ) ;
 * PrintCmd    ::= "print" ( Path | Filename | VARIABLE ) ;
 * ExecCmd     ::= "exec" ( Path | Filename | VARIABLE ) ;
 * 
 * Expr        ::= Term { ( "+" | "-" ) Term } ;
 * Term        ::= Factor { ( "*" | "/" ) Factor } ;
 * Factor      ::= VARIABLE | Filename | Path | "(" Expr ")" ;
 * 
 * Path        ::= "\" { Char } ;
 * Filename    ::= Name "." Ext ;
 * Name        ::= 1*8( Alphanumeric ) ;
 * Ext         ::= 3( Alphanumeric ) ;
 * VARIABLE    ::= "$" ( Letter | Digit ) { Letter | Digit } ;
 * 
 * =========================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cctype>
#include <stdexcept>

// Global Symbol Table Dictionary for Shell Variables
std::unordered_map<std::string, std::string> symbol_table;

enum class TokenType {
    LS, CD, CAT, PRINT, EXEC, SET, ECHO,
    VARIABLE, PATH, FILENAME,
    ASSIGN, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN,
    END
};

struct Token {
    TokenType type;
    std::string value;
};

struct ASTNode {
    std::string command_type;
    std::optional<std::string> argument;
    std::optional<std::string> expression;
};

// Helper: DOS 8.3 Filename Validation
bool isValidDos83(const std::string& str) {
    size_t dot_pos = str.find('.');
    if (dot_pos == std::string::npos) return false;
    
    std::string name = str.substr(0, dot_pos);
    std::string ext = str.substr(dot_pos + 1);
    
    if (name.empty() || name.length() > 8) return false;
    if (ext.length() != 3) return false;
    
    for (char c : name) if (!std::isalnum(c)) return false;
    for (char c : ext) if (!std::isalnum(c)) return false;
    
    return true;
}

// Tokenizer / Lexer
std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    size_t n = input.length();

    while (i < n) {
        if (std::isspace(input[i])) {
            i++;
            continue;
        }

        // Single Character Operators and Delimiters
        if (input[i] == '=') { tokens.push_back({TokenType::ASSIGN, "="}); i++; continue; }
        if (input[i] == '+') { tokens.push_back({TokenType::PLUS, "+"}); i++; continue; }
        if (input[i] == '-') { tokens.push_back({TokenType::MINUS, "-"}); i++; continue; }
        if (input[i] == '*') { tokens.push_back({TokenType::MUL, "*"}); i++; continue; }
        if (input[i] == '/') { tokens.push_back({TokenType::DIV, "/"}); i++; continue; }
        if (input[i] == '(') { tokens.push_back({TokenType::LPAREN, "("}); i++; continue; }
        if (input[i] == ')') { tokens.push_back({TokenType::RPAREN, ")"}); i++; continue; }

        // Variables starting with $
        if (input[i] == '$') {
            std::string var_name = "$";
            i++;
            while (i < n && std::isalnum(input[i])) {
                var_name += input[i];
                i++;
            }
            if (var_name.length() == 1) {
                throw std::runtime_error("Lexer error: lonely '$' without variable identifier");
            }
            tokens.push_back({TokenType::VARIABLE, var_name});
            continue;
        }

        // Paths starting with '\\'
        if (input[i] == '\\') {
            std::string path_str = "\\";
            i++;
            while (i < n && !std::isspace(input[i]) && input[i] != '=' && input[i] != '+' && 
                   input[i] != '-' && input[i] != '*' && input[i] != '/' && input[i] != ')' && input[i] != '(') {
                path_str += input[i];
                i++;
            }
            tokens.push_back({TokenType::PATH, path_str});
            continue;
        }

        // Alphanumeric Identifiers, Keywords, or Filenames
        if (std::isalnum(input[i]) || input[i] == '.') {
            std::string word;
            while (i < n && (std::isalnum(input[i]) || input[i] == '.')) {
                word += input[i];
                i++;
            }

            // Keyword Matching (Case-Insensitive)
            std::string lower_word = word;
            for (char &c : lower_word) c = std::tolower(c);

            if (lower_word == "ls") tokens.push_back({TokenType::LS, "ls"});
            else if (lower_word == "cd") tokens.push_back({TokenType::CD, "cd"});
            else if (lower_word == "cat") tokens.push_back({TokenType::CAT, "cat"});
            else if (lower_word == "print") tokens.push_back({TokenType::PRINT, "print"});
            else if (lower_word == "exec") tokens.push_back({TokenType::EXEC, "exec"});
            else if (lower_word == "set") tokens.push_back({TokenType::SET, "set"});
            else if (lower_word == "echo") tokens.push_back({TokenType::ECHO, "echo"});
            else if (isValidDos83(word)) {
                tokens.push_back({TokenType::FILENAME, word});
            } else {
                throw std::runtime_error("Lexer error: invalid token or filename '" + word + "'");
            }
            continue;
        } // Closes 'if (std::isalnum...)'

        throw std::runtime_error(std::string("Lexer error: unrecognized character '") + input[i] + "'");
    } // Closes 'while (i < n)'

    tokens.push_back({TokenType::END, ""});
    return tokens;
} // Closes 'tokenize()'

// Tokenizer State Machine
class Tokenizer {
private:
    std::vector<Token> tokens;
    size_t index;

public:
    explicit Tokenizer(const std::vector<Token>& token_stream) : tokens(token_stream), index(0) {}

    Token currentToken() const {
        if (index < tokens.size()) return tokens[index];
        return {TokenType::END, ""};
    }

    void nextToken() {
        if (index < tokens.size()) index++;
    }
};

// Recursive Descent Parser Class
class Parser {
private:
    Tokenizer& tokenizer;

    void expect(TokenType type, const std::string& err_msg) {
        if (tokenizer.currentToken().type != type) {
            throw std::runtime_error("Syntax error: " + err_msg);
        }
        tokenizer.nextToken();
    }

    // Expr ::= Term { ("+" | "-") Term }
    std::string parseExpr() {
        std::string left = parseTerm();
        while (tokenizer.currentToken().type == TokenType::PLUS || tokenizer.currentToken().type == TokenType::MINUS) {
            std::string op = tokenizer.currentToken().value;
            tokenizer.nextToken();
            std::string right = parseTerm();
            left = "(" + left + " " + op + " " + right + ")";
        }
        return left;
    }

    // Term ::= Factor { ("*" | "/") Factor }
    std::string parseTerm() {
        std::string left = parseFactor();
        while (tokenizer.currentToken().type == TokenType::MUL || tokenizer.currentToken().type == TokenType::DIV) {
            std::string op = tokenizer.currentToken().value;
            tokenizer.nextToken();
            std::string right = parseFactor();
            left = "(" + left + " " + op + " " + right + ")";
        }
        return left;
    }

    // Factor ::= VARIABLE | FILENAME | PATH | "(" Expr ")"
    std::string parseFactor() {
        Token cur = tokenizer.currentToken();
        if (cur.type == TokenType::VARIABLE || cur.type == TokenType::FILENAME || cur.type == TokenType::PATH) {
            tokenizer.nextToken();
            return cur.value;
        } else if (cur.type == TokenType::LPAREN) {
            expect(TokenType::LPAREN, "expected '('");
            std::string inner_expr = parseExpr();
            expect(TokenType::RPAREN, "expected ')'");
            return inner_expr;
        }
        throw std::runtime_error("Syntax error: expected variable, filename, path, or '(' in expression");
    }

    // SetCmd ::= "set" VARIABLE "=" Expr
    ASTNode parseSet() {
        expect(TokenType::SET, "expected 'set'");
        if (tokenizer.currentToken().type != TokenType::VARIABLE) {
            throw std::runtime_error("'set' requires a $VARIABLE identifier");
        }
        std::string var_name = tokenizer.currentToken().value;
        tokenizer.nextToken();

        expect(TokenType::ASSIGN, "expected '=' after variable name");
        std::string expr_str = parseExpr();

        // Update/Define variable in Symbol Table
        symbol_table[var_name] = expr_str;

        expect(TokenType::END, "extra trailing tokens after SET statement");
        return {"SetCommand", var_name, expr_str};
    }

    // EchoCmd ::= "echo" ( VARIABLE | Expr )
    ASTNode parseEcho() {
        expect(TokenType::ECHO, "expected 'echo'");
        std::string expr_str = parseExpr();
        expect(TokenType::END, "extra trailing tokens after ECHO statement");
        return {"EchoCommand", std::nullopt, expr_str};
    }

    ASTNode parseLs() {
        expect(TokenType::LS, "expected 'ls'");
        Token next = tokenizer.currentToken();
        std::optional<std::string> arg = std::nullopt;
        if (next.type == TokenType::PATH || next.type == TokenType::FILENAME || next.type == TokenType::VARIABLE) {
            arg = next.value;
            tokenizer.nextToken();
        }
        expect(TokenType::END, "unexpected tokens after 'ls' command");
        return {"LsCommand", arg, std::nullopt};
    }

    ASTNode parseCd() {
        expect(TokenType::CD, "expected 'cd'");
        Token next = tokenizer.currentToken();
        std::optional<std::string> arg = std::nullopt;
        if (next.type == TokenType::PATH || next.type == TokenType::VARIABLE) {
            arg = next.value;
            tokenizer.nextToken();
        } else if (next.type == TokenType::FILENAME) {
            throw std::runtime_error("'cd' expects a path, not a filename");
        }
        expect(TokenType::END, "unexpected tokens after 'cd' command");
        return {"CdCommand", arg, std::nullopt};
    }

    ASTNode parseCat() {
        expect(TokenType::CAT, "expected 'cat'");
        Token next = tokenizer.currentToken();
        if (next.type == TokenType::FILENAME || next.type == TokenType::PATH || next.type == TokenType::VARIABLE) {
            std::string arg = next.value;
            tokenizer.nextToken();
            expect(TokenType::END, "unexpected tokens after 'cat' command");
            return {"CatCommand", arg, std::nullopt};
        }
        throw std::runtime_error("'cat' requires a filename, path, or $VARIABLE");
    }

    ASTNode parsePrint() {
        expect(TokenType::PRINT, "expected 'print'");
        Token next = tokenizer.currentToken();
        if (next.type == TokenType::FILENAME || next.type == TokenType::PATH || next.type == TokenType::VARIABLE) {
            std::string arg = next.value;
            tokenizer.nextToken();
            expect(TokenType::END, "unexpected tokens after 'print' command");
            return {"PrintCommand", arg, std::nullopt};
        }
        throw std::runtime_error("'print' requires a filename, path, or $VARIABLE");
    }

    ASTNode parseExec() {
        expect(TokenType::EXEC, "expected 'exec'");
        Token next = tokenizer.currentToken();
        if (next.type == TokenType::FILENAME || next.type == TokenType::PATH || next.type == TokenType::VARIABLE) {
            std::string arg = next.value;
            tokenizer.nextToken();
            expect(TokenType::END, "unexpected tokens after 'exec' command");
            return {"ExecCommand", arg, std::nullopt};
        }
        throw std::runtime_error("'exec' requires an executable filename, path, or $VARIABLE");
    }

public:
    explicit Parser(Tokenizer& tok) : tokenizer(tok) {}

    ASTNode parse() {
        Token cur = tokenizer.currentToken();
        switch (cur.type) {
            case TokenType::LS:    return parseLs();
            case TokenType::CD:    return parseCd();
            case TokenType::CAT:   return parseCat();
            case TokenType::PRINT: return parsePrint();
            case TokenType::EXEC:  return parseExec();
            case TokenType::SET:   return parseSet();
            case TokenType::ECHO:  return parseEcho();
            default:
                throw std::runtime_error("Syntax error: unknown command keyword");
        }
    }

    void printSymbolTable() const {
        std::cout << "\n  [ SYMBOL TABLE ]" << std::endl;
        if (symbol_table.empty()) {
            std::cout << "  (empty)" << std::endl;
        } else {
            for (const auto& [var, val] : symbol_table) {
                std::cout << "  " << var << " = " << val << std::endl;
            }
        }
        std::cout << "  ----------------\n" << std::endl;
    }
};

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "   CS 414 Assignment 04 Shell Parser     " << std::endl;
    std::cout << "=========================================\n" << std::endl;

    std::string input;
    while (true) {
        std::cout << "shell> ";
        if (!std::getline(std::cin, input)) break;
        if (input == "exit" || input == "quit") break;
        if (input.empty()) continue;

        try {
            std::vector<Token> tokens = tokenize(input);
            Tokenizer tok(tokens);
            Parser parser(tok);

            ASTNode ast = parser.parse();
            std::cout << "[AST Node] Command: " << ast.command_type;
            if (ast.argument.has_value()) {
                std::cout << " | Argument: \"" << ast.argument.value() << "\"";
            }
            if (ast.expression.has_value()) {
                std::cout << " | Expr Tree: \"" << ast.expression.value() << "\"";
            }
            std::cout << std::endl;

            // Print Symbol Table after every parse
            parser.printSymbolTable();

        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << std::endl;
        }
    }

    return 0;
}