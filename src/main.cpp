#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant> // 支持 std::variant

// Token 类型
enum class TokenType {
    IDENTIFIER,  // 变量名
    NUMBER,      // 数字（整数或浮点数）
    PLUS,        // +
    MINUS,       // -
    MULTIPLY,    // *
    DIVIDE,      // /
    ASSIGN,      // =
    SEMICOLON,   // ;
    LPAREN,      // (
    RPAREN,      // )
    IF,          // if
    INT,         // int
    CHAR,        // char
    LBRACE,      // {
    RBRACE,      // }
    LT,          // <
    GT,          // >
    LE,          // <=
    GE,          // >=
    EQ,          // ==
    NEQ,         // !=
    CHAR_LITERAL,// 字符常量，如 'a'
    END          // 输入结束
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
private:
    std::string text;
    size_t pos; // 改为 size_t
    char current_char;

    void advance() {
        pos++;
        if (pos >= text.length()) {
            current_char = '\0';
        } else {
            current_char = text[pos];
        }
    }

    void skip_whitespace() {
        while (current_char != '\0' && std::isspace(current_char)) {
            advance();
        }
    }

public:
    Lexer(const std::string& input_text) : text(input_text), pos(0), current_char(input_text.empty() ? '\0' : input_text[0]) {}

    Token get_next_token() {
        while (current_char != '\0') {
            if (std::isspace(current_char)) {
                skip_whitespace();
                continue;
            }

            // 标识符和关键字
            if (std::isalpha(current_char) || current_char == '_') {
                std::string identifier_str;
                while (current_char != '\0' && (std::isalnum(current_char) || current_char == '_')) {
                    identifier_str += current_char;
                    advance();
                }
                if (identifier_str == "if") return {TokenType::IF, identifier_str};
                if (identifier_str == "int") return {TokenType::INT, identifier_str};
                if (identifier_str == "char") return {TokenType::CHAR, identifier_str};
                return {TokenType::IDENTIFIER, identifier_str};
            }

            // 数字（整数或浮点数）
            if (std::isdigit(current_char)) {
                std::string number_str;
                bool has_dot = false;
                while (current_char != '\0' && (std::isdigit(current_char) || current_char == '.')) {
                    if (current_char == '.') {
                        if (has_dot) {
                            std::cerr << "Invalid number: multiple decimal points" << std::endl;
                            return {TokenType::END, number_str};
                        }
                        has_dot = true;
                    }
                    number_str += current_char;
                    advance();
                }
                return {TokenType::NUMBER, number_str};
            }

            // 字符常量，如 'a'
            if (current_char == '\'') {
                advance();
                if (current_char == '\0' || current_char == '\'') {
                    std::cerr << "Invalid character literal" << std::endl;
                    return {TokenType::END, ""};
                }
                std::string char_str(1, current_char);
                advance();
                if (current_char != '\'') {
                    std::cerr << "Expected closing single quote" << std::endl;
                    return {TokenType::END, ""};
                }
                advance();
                return {TokenType::CHAR_LITERAL, char_str};
            }

            // 运算符和界符
            if (current_char == '+') {
                advance();
                return {TokenType::PLUS, "+"};
            }
            if (current_char == '-') {
                advance();
                return {TokenType::MINUS, "-"};
            }
            if (current_char == '*') {
                advance();
                return {TokenType::MULTIPLY, "*"};
            }
            if (current_char == '/') {
                advance();
                return {TokenType::DIVIDE, "/"};
            }
            if (current_char == '=') {
                if (pos + 1 < text.length() && text[pos + 1] == '=') {
                    advance();
                    advance();
                    return {TokenType::EQ, "=="};
                }
                advance();
                return {TokenType::ASSIGN, "="};
            }
            if (current_char == ';') {
                advance();
                return {TokenType::SEMICOLON, ";"};
            }
            if (current_char == '(') {
                advance();
                return {TokenType::LPAREN, "("};
            }
            if (current_char == ')') {
                advance();
                return {TokenType::RPAREN, ")"};
            }
            if (current_char == '{') {
                advance();
                return {TokenType::LBRACE, "{"};
            }
            if (current_char == '}') {
                advance();
                return {TokenType::RBRACE, "}"};
            }
            if (current_char == '<') {
                if (pos + 1 < text.length() && text[pos + 1] == '=') {
                    advance();
                    advance();
                    return {TokenType::LE, "<="};
                }
                advance();
                return {TokenType::LT, "<"};
            }
            if (current_char == '>') {
                if (pos + 1 < text.length() && text[pos + 1] == '=') {
                    advance();
                    advance();
                    return {TokenType::GE, ">="};
                }
                advance();
                return {TokenType::GT, ">"};
            }
            if (current_char == '!') {
                if (pos + 1 < text.length() && text[pos + 1] == '=') {
                    advance();
                    advance();
                    return {TokenType::NEQ, "!="};
                }
                std::cerr << "Invalid character sequence starting with '!'" << std::endl;
                return {TokenType::END, ""};
            }

            std::cerr << "Invalid character: " << current_char << std::endl;
            advance();
            return {TokenType::END, ""};
        }
        return {TokenType::END, ""};
    }
};

class SymbolTable {
public:
    std::unordered_map<std::string, std::variant<int, float, char>> symbols;

    void set_value(const std::string& identifier, std::variant<int, float, char> value) {
        symbols[identifier] = value;
    }

    std::variant<int, float, char> get_value(const std::string& identifier) {
        auto it = symbols.find(identifier);
        if (it != symbols.end()) {
            return it->second;
        }
        std::cerr << "Variable " << identifier << " not declared." << std::endl;
        return 0;
    }
};

class Parser {
private:
    Lexer lexer;
    Token current_token;
    SymbolTable symbol_table;

    void eat(TokenType token_type) {
        if (current_token.type == token_type) {
            current_token = lexer.get_next_token();
        } else {
            std::cerr << "Unexpected token. Expected: " << static_cast<int>(token_type)
                      << ", got: " << static_cast<int>(current_token.type) << " (" << current_token.value << ")" << std::endl;
            current_token = lexer.get_next_token();
        }
    }

    std::variant<int, float, char> factor() {
        Token token = current_token;
        if (token.type == TokenType::NUMBER) {
            eat(TokenType::NUMBER);
            if (token.value.find('.') != std::string::npos) {
                return std::stof(token.value);
            }
            return std::stoi(token.value);
        } else if (token.type == TokenType::CHAR_LITERAL) {
            eat(TokenType::CHAR_LITERAL);
            if (token.value.length() == 1) {
                return token.value[0];
            }
            std::cerr << "Invalid character literal: " << token.value << std::endl;
            return char(0);
        } else if (token.type == TokenType::IDENTIFIER) {
            eat(TokenType::IDENTIFIER);
            return symbol_table.get_value(token.value);
        } else if (token.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            auto result = expression();
            eat(TokenType::RPAREN);
            return result;
        }
        return 0;
    }

    std::variant<int, float, char> term() {
        auto result = factor();
        while (current_token.type == TokenType::MULTIPLY || current_token.type == TokenType::DIVIDE) {
            if (current_token.type == TokenType::MULTIPLY) {
                eat(TokenType::MULTIPLY);
                auto next = factor();
                if (std::holds_alternative<int>(result) && std::holds_alternative<int>(next)) {
                    result = std::get<int>(result) * std::get<int>(next);
                } else if (std::holds_alternative<float>(result) || std::holds_alternative<float>(next)) {
                    float left = std::holds_alternative<float>(result) ? std::get<float>(result) : std::get<int>(result);
                    float right = std::holds_alternative<float>(next) ? std::get<float>(next) : std::get<int>(next);
                    result = left * right;
                } else {
                    std::cerr << "Multiplication not supported for char" << std::endl;
                    return 0;
                }
            } else if (current_token.type == TokenType::DIVIDE) {
                eat(TokenType::DIVIDE);
                auto next = factor();
                if (std::holds_alternative<int>(result) && std::holds_alternative<int>(next)) {
                    result = std::get<int>(result) / std::get<int>(next);
                } else if (std::holds_alternative<float>(result) || std::holds_alternative<float>(next)) {
                    float left = std::holds_alternative<float>(result) ? std::get<float>(result) : std::get<int>(result);
                    float right = std::holds_alternative<float>(next) ? std::get<float>(next) : std::get<int>(next);
                    result = left / right;
                } else {
                    std::cerr << "Division not supported for char" << std::endl;
                    return 0;
                }
            }
        }
        return result;
    }

    std::variant<int, float, char> expression() {
        auto result = term();
        while (current_token.type == TokenType::PLUS || current_token.type == TokenType::MINUS) {
            if (current_token.type == TokenType::PLUS) {
                eat(TokenType::PLUS);
                auto next = term();
                if (std::holds_alternative<int>(result) && std::holds_alternative<int>(next)) {
                    result = std::get<int>(result) + std::get<int>(next);
                } else if (std::holds_alternative<float>(result) || std::holds_alternative<float>(next)) {
                    float left = std::holds_alternative<float>(result) ? std::get<float>(result) : std::get<int>(result);
                    float right = std::holds_alternative<float>(next) ? std::get<float>(next) : std::get<int>(next);
                    result = left + right;
                } else {
                    std::cerr << "Addition not supported for char" << std::endl;
                    return 0;
                }
            } else {
                eat(TokenType::MINUS);
                auto next = term();
                if (std::holds_alternative<int>(result) && std::holds_alternative<int>(next)) {
                    result = std::get<int>(result) - std::get<int>(next);
                } else if (std::holds_alternative<float>(result) || std::holds_alternative<float>(next)) {
                    float left = std::holds_alternative<float>(result) ? std::get<float>(result) : std::get<int>(result);
                    float right = std::holds_alternative<float>(next) ? std::get<float>(next) : std::get<int>(next);
                    result = left - right;
                } else {
                    std::cerr << "Subtraction not supported for char" << std::endl;
                    return 0;
                }
            }
        }
        return result;
    }

    bool condition() {
        auto left = expression();
        TokenType op = current_token.type;
        if (op == TokenType::LT || op == TokenType::GT || op == TokenType::LE || op == TokenType::GE || op == TokenType::EQ || op == TokenType::NEQ) {
            eat(op);
            auto right = expression();
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                int l = std::get<int>(left), r = std::get<int>(right);
                if (op == TokenType::LT) return l < r;
                if (op == TokenType::GT) return l > r;
                if (op == TokenType::LE) return l <= r;
                if (op == TokenType::GE) return l >= r;
                if (op == TokenType::EQ) return l == r;
                if (op == TokenType::NEQ) return l != r;
            } else if (std::holds_alternative<float>(left) || std::holds_alternative<float>(right)) {
                float l = std::holds_alternative<float>(left) ? std::get<float>(left) : std::get<int>(left);
                float r = std::holds_alternative<float>(right) ? std::get<float>(right) : std::get<int>(right);
                if (op == TokenType::LT) return l < r;
                if (op == TokenType::GT) return l > r;
                if (op == TokenType::LE) return l <= r;
                if (op == TokenType::GE) return l >= r;
                if (op == TokenType::EQ) return l == r;
                if (op == TokenType::NEQ) return l != r;
            } else {
                std::cerr << "Comparison not supported for char" << std::endl;
                return false;
            }
        }
        return false;
    }

    void statement() {
        if (current_token.type == TokenType::INT || current_token.type == TokenType::CHAR) {
            TokenType type = current_token.type;
            eat(type);
            if (current_token.type == TokenType::IDENTIFIER) {
                std::string identifier = current_token.value;
                eat(TokenType::IDENTIFIER);
                eat(TokenType::ASSIGN);
                auto value = expression();
                symbol_table.set_value(identifier, value);
                eat(TokenType::SEMICOLON);
            }
        } else if (current_token.type == TokenType::IDENTIFIER) {
            std::string identifier = current_token.value;
            eat(TokenType::IDENTIFIER);
            eat(TokenType::ASSIGN);
            auto value = expression();
            symbol_table.set_value(identifier, value);
            eat(TokenType::SEMICOLON);
        } else if (current_token.type == TokenType::IF) {
            eat(TokenType::IF);
            eat(TokenType::LPAREN);
            bool condition_value = condition();
            eat(TokenType::RPAREN);
            if (condition_value) {
                eat(TokenType::LBRACE);
                block();
                eat(TokenType::RBRACE);
            } else {
                eat(TokenType::LBRACE);
                while (current_token.type != TokenType::RBRACE && current_token.type != TokenType::END) {
                    eat(current_token.type);
                }
                eat(TokenType::RBRACE);
            }
        }
    }

    void block() {
        while (current_token.type != TokenType::RBRACE && current_token.type != TokenType::END) {
            statement();
        }
    }

public:
    Parser(const std::string& text) : lexer(text), current_token(lexer.get_next_token()) {}

    void parse() {
        while (current_token.type != TokenType::END) {
            statement();
        }
    }

    SymbolTable& get_symbol_table() {
        return symbol_table;
    }
};

int main() {
    std::string input = "int x = 5; char c = 'a'; if (x > 3) { x = x + 1; }";
    Parser parser(input);
    parser.parse();
    SymbolTable& symbols = parser.get_symbol_table();
    auto x = symbols.get_value("x");
    auto c = symbols.get_value("c");
    std::cout << "x = ";
    if (std::holds_alternative<int>(x)) std::cout << std::get<int>(x);
    else if (std::holds_alternative<float>(x)) std::cout << std::get<float>(x);
    else std::cout << "'" << std::get<char>(x) << "'";
    std::cout << std::endl;
    std::cout << "c = ";
    if (std::holds_alternative<int>(c)) std::cout << std::get<int>(c);
    else if (std::holds_alternative<float>(c)) std::cout << std::get<float>(c);
    else std::cout << "'" << std::get<char>(c) << "'";
    std::cout << std::endl;
    return 0;
}