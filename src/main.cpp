#include <iostream>
#include <string>
#include <vector>
#include <cctype>

// 简单的类型枚举
enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    UNKNOWN
};

// 表示一个单词（令牌）
struct Token
{
    TokenType type;
    std::string value;
    // 新增：存储在相同类型数组里的下标标号
    size_t indexInTypeArray=100;
};

// 简单的关键字列表
const std::vector<std::string> keywords = 
{
    "if", "else", "for", "while", "int", "float", "double", "return"
};

// 判断是否是关键字
bool isKeyword(const std::string& word)
{
    for (const auto& keyword : keywords) {
        if (keyword == word) {
            return true;
        }
    }
    return false;
}

// 判断是否是运算符
bool isOperator(char c)
{
    return std::string("+-*/=<>!&|").find(c) != std::string::npos;
}

// 解析代码字符串为单词向量，并记录每个单词类型在相同类型数组里的下标标号
std::vector<Token> tokenize(const std::string& code)
{
    std::vector<Token> tokens;

    // 分别用于记录每种类型单词出现的次数，用于确定下标标号
    size_t keywordCount = 0;
    size_t identifierCount = 0;
    size_t numberCount = 0;
    size_t operatorCount = 0;

    std::string currentWord;
    for (size_t i = 0; i < code.length(); ++i)
    {
        char c = code[i];
        if (std::isspace(c))
        {
            if (!currentWord.empty())
            {
                Token token;
                if (isKeyword(currentWord))
                {
                    token.type = TokenType::KEYWORD;
                    token.indexInTypeArray = keywordCount++;
                }
                else if (std::isdigit(currentWord[0]))
                {
                    token.type = TokenType::NUMBER;
                    
                        
                    token.indexInTypeArray = numberCount++;
                }
                else if (isOperator(currentWord[0]))
                {
                    token.type = TokenType::OPERATOR;
                    token.indexInTypeArray = operatorCount++;
                }
                else
                {
                    token.type = TokenType::IDENTIFIER;
                    token.indexInTypeArray = identifierCount++;
                }
                token.value = currentWord;
                tokens.push_back(token);
                currentWord.clear();
            }
        }
        else if (isOperator(c))
        {
            if (!currentWord.empty())
            {
                Token token;
                if (isKeyword(currentWord))
                {
                    token.type = TokenType::KEYWORD;
                    token.indexInTypeArray = keywordCount++;
                }
                else if (std::isdigit(currentWord[0]))
                {
                    token.type = TokenType::NUMBER;
                    token.indexInTypeArray = numberCount++;
                }
                else
                {
                    token.type = TokenType::IDENTIFIER;
                    token.indexInTypeArray = identifierCount++;
                }
                token.value = currentWord;
                tokens.push_back(token);
                currentWord.clear();
            }
            currentWord += c;
            Token operatorToken;
            operatorToken.type = TokenType::OPERATOR;
            operatorToken.indexInTypeArray = operatorCount++;
            operatorToken.value = currentWord;
            tokens.push_back(operatorToken);
            currentWord.clear();
        }
        else
        {
            currentWord += c;
        }
    }
    if (!currentWord.empty())
    {
        Token token;
        if (isKeyword(currentWord))
        {
            token.type = TokenType::KEYWORD;
            token.indexInTypeArray = keywordCount++;
        }
        else if (std::isdigit(currentWord[0]))
        {
            token.type = TokenType::NUMBER;
            token.indexInTypeArray = numberCount++;
        }
        else if (isOperator(currentWord[0]))
        {
            token.type = TokenType::OPERATOR;
            token.indexInTypeArray = operatorCount++;
        }
        else
        {
            token.type = TokenType::IDENTIFIER;
            token.indexInTypeArray = identifierCount++;
        }
        token.value = currentWord;
        tokens.push_back(token);
    }

    return tokens;
}

int main()
{
    std::string code;
    std::cout << "请输入代码: ";
    std::getline(std::cin, code);
    std::vector<Token> tokens = tokenize(code);
    for (const auto& token : tokens) {
        std::string typeStr;
        switch (token.type)
        {
        case TokenType::KEYWORD:
            typeStr = "关键字";
            break;
        case TokenType::IDENTIFIER:
            typeStr = "标识符";
            break;
        case TokenType::NUMBER:
            typeStr = "数字";
            break;
        case TokenType::OPERATOR:
            typeStr = "运算符";
            break;
        default:
            typeStr = "未知类型";
            break;
        }
        std::cout << "单词: " << token.value << ", 类型: " << typeStr << ", 在同类型数组里的下标标号: " << token.indexInTypeArray << std::endl;
    }
    return 0;
}