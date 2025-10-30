#include "Lex.h"
#include <fstream>
#include <iostream>
#include <cctype>

Lexer::Lexer(const std::string& filename) : fileName(filename), currentLine(1), currentPos(0) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            sourceCode += line + "\n";
            currentLine++;
        }
        file.close();
    } else {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    initializeTransitionTable();
    initializeKeywordMap();
}

Lexer::~Lexer() {
}

void Lexer::initializeTransitionTable() {
    for (int i = 0; i < 128; i++) {
        if (isalpha(i)) transTable[0][i].push_back(1); // 字母 -> 状态1
        else if (isdigit(i)) transTable[0][i].push_back(2); // 数字 -> 状态2
        else if (i == '+') transTable[0][i].push_back(-3); // '+' -> 终止状态-3
        else transTable[0][i].push_back(0); // 其他字符保持状态0
    }
    transTable[1][0].push_back(-1); // 状态1结束
    transTable[2][0].push_back(-2); // 状态2结束
}

void Lexer::initializeKeywordMap() {
    keywordMap["if"] = 1;
    keywordMap["else"] = 2;
    keywordMap["while"] = 3;
}

int Lexer::getNextState(int state, char ch) {
    int ascii = static_cast<unsigned char>(ch);
    if (state >= 0 && state < 50 && ascii < 128 && !transTable[state][ascii].empty()) {
        return transTable[state][ascii][0];
    }
    std::cout << "Invalid char: " << ch << " at state " << state << std::endl;
    return -1; // 无效字符终止
}

void Lexer::processToken(int state, std::string& lexeme) {
    Token token;
    token.lineNumber = currentLine;
    if (state == -1) {
        token.description = keywordMap.count(lexeme) ? "Keyword: " + lexeme : "Identifier: " + lexeme;
        token.tokenId = keywordMap.count(lexeme) ? keywordMap[lexeme] : 4;
    } else if (state == -2) {
        token.description = "Constant: " + lexeme;
        token.tokenId = 5;
    } else if (state == -3) {
        token.description = "Operator: " + lexeme;
        token.tokenId = 6;
    }
    tokenStream.push_back(token);
}

bool Lexer::analyze() {
    if (sourceCode.empty()) {
        std::cerr << "源代码为空，分析终止。\n";
        return false;
    }

    currentPos = 0;
    currentLine = 1;
    int state = 0;
    std::string lexeme = "";

    std::cout << "Starting analysis of " << sourceCode.length() << " characters.\n";
    while (currentPos < sourceCode.length()) {
        char ch = sourceCode[currentPos];
        int nextState = getNextState(state, ch);
        std::cout << "Pos: " << currentPos << ", Char: " << ch << ", State: " << state << " -> " << nextState << std::endl;

        if (nextState < 0) { // 到达终止状态
            if (!lexeme.empty() || nextState != 0) {
                processToken(nextState, lexeme);
            }
            state = 0;
            lexeme = "";
            currentPos++; // 确保前进到下一个字符
            continue; // 跳到下一次循环
        } else {
            state = nextState;
            lexeme += ch;
        }
        currentPos++;
    }

    if (!lexeme.empty()) {
        processToken(-1, lexeme); // 处理最后一个单词
    }
    return true;
}

void Lexer::printTokenStream() {
    if (tokenStream.empty()) {
        std::cout << "无单词流输出。\n";
        return;
    }
    for (const auto& token : tokenStream) {
        std::cout << "(" << token.tokenId << ", " << token.description << ", " << token.lineNumber << ")\n";
    }
}