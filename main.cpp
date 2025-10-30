#include "Lex.h"
#include <iostream>

int main() {
    std::string filename = "source.txt";
    Lexer lexer(filename);

    if (lexer.analyze()) {
        std::cout << "词法分析完成，输出单词流:\n";
        lexer.printTokenStream();
    } else {
        std::cout << "词法分析失败。\n";
    }

    return 0;
}