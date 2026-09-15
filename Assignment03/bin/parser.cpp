/*
 * Parser.cpp
 * Assignment 03
 * Hand-built recursive descent parser for shell commands
 */

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cctype>
#include <stdexcept>

// Token categories for the shell grammar
enum class TokenType {
    LS,
    CD,
    CAT,
    PRINT,
    EXEC,
    PATH,
    FILENAME,
    END
};

// Token representation
struct Token {
    TokenType type;
    std::string value;
};

// Abstract Syntax Tree node structure
struct ASTNode {
    std::string command_type;
    std::optional<std::string> argument;
};

// Helper function to validate DOS 8.3 character rules
bool isDosChar(char c) {
    return std::isalnum(c);
}

// Standalone scanner function to tokenize raw input string
std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    size_t len = input.length();

    while (i < len) {
        if (std::isspace(input[i])) {
            i++;
            continue;
        }

        // Process path arguments starting with backslash
        if (input[i] == '\\') {
            std::string path_val = "";
            while (i < len && !std::isspace(input[i])) {
                path_val += input[i];
                i++;
            }
            tokens.push_back({TokenType::PATH, path_val});
            continue;
        }

        // Extract alphanumeric words
        std::string word = "";
        while (i < len && !std::isspace(input[i])) {
            word += input[i];
            i++;
        }

        // Match keyword tokens
        if (word == "ls") {
            tokens.push_back({TokenType::LS, word});
        } else if (word == "cd") {
            tokens.push_back({TokenType::CD, word});
        } else if (word == "cat") {
            tokens.push_back({TokenType::CAT, word});
        } else if (word == "print") {
            tokens.push_back({TokenType::PRINT, word});
        } else if (word == "exec") {
            tokens.push_back({TokenType::EXEC, word});
        } else {
            // Validate DOS 8.3 filename format (NAME.EXT)
            size_t dot_pos = word.find('.');
            if (dot_pos != std::string::npos) {
                std::string name = word.substr(0, dot_pos);
                std::string ext = word.substr(dot_pos + 1);

                if (name.length() >= 1 && name.length() <= 8 && ext.length() == 3) {
                    bool valid = true;
                    for (char c : name) {
                        if (!isDosChar(c)) valid = false;
                    }
                    for (char c : ext) {
                        if (!isDosChar(c)) valid = false;
                    }

                    if (valid) {
                        tokens.push_back({TokenType::FILENAME, word});
                        continue;
                    }
                }
            }
            throw std::runtime_error("Lexer error: invalid token or filename '" + word + "'");
        }
    }

    tokens.push_back({TokenType::END, ""});
    return tokens;
}

// Tokenizer state machine wrapper
class Tokenizer {
private:
    std::vector<Token> tokens;
    size_t index;

public:
    explicit Tokenizer(const std::vector<Token>& token_list) 
        : tokens(token_list), index(0) {}

    Token currentToken() const {
        if (index < tokens.size()) {
            return tokens[index];
        }
        return {TokenType::END, ""};
    }

    void nextToken() {
        if (index < tokens.size()) {
            index++;
        }
    }
};

// Recursive Descent Parser mirroring instructor style
class Parser {
public:
    explicit Parser(Tokenizer& tokenizer) : tokenizer(tokenizer) {}

    ASTNode parse() {
        return parseCommand();
    }

private:
    Tokenizer& tokenizer;

    // Helper to consume expected token or throw error
    void expect(TokenType type, const std::string& err_msg) {
        if (tokenizer.currentToken().type != type) {
            throw std::runtime_error("Syntax error: " + err_msg);
        }
        tokenizer.nextToken();
    }

    // Command -> LsCmd | CdCmd | CatCmd | PrintCmd | ExecCmd
    ASTNode parseCommand() {
        TokenType current = tokenizer.currentToken().type;

        if (current == TokenType::LS) {
            return parseLs();
        } else if (current == TokenType::CD) {
            return parseCd();
        } else if (current == TokenType::CAT) {
            return parseCat();
        } else if (current == TokenType::PRINT) {
            return parsePrint();
        } else if (current == TokenType::EXEC) {
            return parseExec();
        } else {
            throw std::runtime_error("Syntax error: unknown command");
        }
    }

    // LsCmd -> "ls" [ PATH | FILENAME ]
    ASTNode parseLs() {
        expect(TokenType::LS, "expected 'ls'");
        std::optional<std::string> arg = std::nullopt;

        TokenType current = tokenizer.currentToken().type;
        if (current == TokenType::PATH || current == TokenType::FILENAME) {
            arg = tokenizer.currentToken().value;
            tokenizer.nextToken();
        }

        expect(TokenType::END, "unexpected extra arguments after 'ls'");
        return {"LsCommand", arg};
    }

    // CdCmd -> "cd" [ PATH ]
    ASTNode parseCd() {
        expect(TokenType::CD, "expected 'cd'");
        std::optional<std::string> arg = std::nullopt;

        TokenType current = tokenizer.currentToken().type;
        if (current == TokenType::PATH) {
            arg = tokenizer.currentToken().value;
            tokenizer.nextToken();
        } else if (current == TokenType::FILENAME) {
            throw std::runtime_error("'cd' expects a path, not a filename");
        }

        expect(TokenType::END, "unexpected extra arguments after 'cd'");
        return {"CdCommand", arg};
    }

    // CatCmd -> "cat" ( PATH | FILENAME )
    ASTNode parseCat() {
        expect(TokenType::CAT, "expected 'cat'");
        TokenType current = tokenizer.currentToken().type;

        if (current == TokenType::PATH || current == TokenType::FILENAME) {
            std::string arg = tokenizer.currentToken().value;
            tokenizer.nextToken();
            expect(TokenType::END, "unexpected extra arguments after 'cat'");
            return {"CatCommand", arg};
        }
        throw std::runtime_error("'cat' requires a filename or path argument");
    }

    // PrintCmd -> "print" ( PATH | FILENAME )
    ASTNode parsePrint() {
        expect(TokenType::PRINT, "expected 'print'");
        TokenType current = tokenizer.currentToken().type;

        if (current == TokenType::PATH || current == TokenType::FILENAME) {
            std::string arg = tokenizer.currentToken().value;
            tokenizer.nextToken();
            expect(TokenType::END, "unexpected extra arguments after 'print'");
            return {"PrintCommand", arg};
        }
        throw std::runtime_error("'print' requires a filename or path argument");
    }

    // ExecCmd -> "exec" ( PATH | FILENAME )
    ASTNode parseExec() {
        expect(TokenType::EXEC, "expected 'exec'");
        TokenType current = tokenizer.currentToken().type;

        if (current == TokenType::PATH || current == TokenType::FILENAME) {
            std::string arg = tokenizer.currentToken().value;
            tokenizer.nextToken();
            expect(TokenType::END, "unexpected extra arguments after 'exec'");
            return {"ExecCommand", arg};
        }
        throw std::runtime_error("'exec' requires a filename or path argument");
    }
};

// Helper function to print AST content
void printAST(const ASTNode& node) {
    std::cout << "  [AST Node] Command: " << node.command_type;
    if (node.argument.has_value()) {
        std::cout << " | Argument: \"" << node.argument.value() << "\"";
    } else {
        std::cout << " | Argument: (none)";
    }
    std::cout << std::endl;
}

// Entry point main function
int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "        PARSER TEST SUITE" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::vector<std::string> test_commands = {
        "ls",
        "ls \\DOCS",
        "ls PROGRAM1.EXE",
        "cd",
        "cd \\DOCS\\SUB1",
        "cat NOTES.TXT",
        "print REPORT.DOC",
        "exec RUN.EXE",
        "invalid_command",
        "cd MYFILE.TXT",
        "cat",
        "exec TOOLONGNAME.TEXTFILE"
    };

    for (const auto& cmd : test_commands) {
        std::cout << "\nTesting Input: \"" << cmd << "\"" << std::endl;
        try {
            std::vector<Token> token_list = tokenize(cmd);
            Tokenizer tokenizer(token_list);
            Parser parser(tokenizer);
            ASTNode ast = parser.parse();
            printAST(ast);
            std::cout << "  Status: SUCCESS" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  Status: REJECTED -> " << e.what() << std::endl;
        }
    }

    std::cout << "\n==========================================" << std::endl;
    std::cout << "  INTERACTIVE SHELL MODE (type 'exit' to quit)" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::string input;
    while (true) {
        std::cout << "\nshell> ";
        if (!std::getline(std::cin, input) || input == "exit") {
            break;
        }

        if (input.empty()) continue;

        try {
            std::vector<Token> token_list = tokenize(input);
            Tokenizer tokenizer(token_list);
            Parser parser(tokenizer);
            ASTNode ast = parser.parse();
            printAST(ast);
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}