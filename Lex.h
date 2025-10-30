#ifndef LEX_H
#define LEX_H

#include <vector>
#include <string>
#include <map>
#include "Token.h"

class Lexer {
private:
    std::vector<int> transTable[50][128]; // 简化的状态转换表，50个状态，128个ASCII字符
    std::string sourceCode;              // 源代码
    std::map<std::string, int> keywordMap; // 关键字表
    std::vector<Token> tokenStream;       // 单词流
    int currentLine;                      // 当前行号
    int currentPos;                       // 当前位置
    std::string fileName;                 // 文件名

    void initializeTransitionTable();     // 初始化转换表
    void initializeKeywordMap();          // 初始化关键字表
    int getNextState(int state, char ch); // 获取下一个状态
    void processToken(int state, std::string& lexeme); // 处理识别的单词

public:
    Lexer(const std::string& filename);
    ~Lexer();
    bool analyze();                      // 词法分析主函数
    void printTokenStream();             // 输出单词流
};

#endif