#ifndef TOKEN_H
#define TOKEN_H

#include <string>

struct Token {
    int tokenId;        // 单词ID
    std::string description; // 单词备注
    int lineNumber;     // 单词行号

    Token() : tokenId(0), lineNumber(0) {}
    Token(int id, const std::string& desc, int line) 
        : tokenId(id), description(desc), lineNumber(line) {}
};

#endif