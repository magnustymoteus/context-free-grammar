//
// Created by gruzi on 22/10/2023.
//

#ifndef CFG_LLPARSER1_H
#define CFG_LLPARSER1_H

#include "CFG.h"

class LL1Parser {
private:

public:
    std::map<std::string, std::set<std::string>> followSet;
    std::map<std::string, std::set<std::string>> firstSet;
    std::map<std::pair<std::string, std::string>, std::string> parsingTable; // {row, column} : string
    explicit LL1Parser(const CFG &cfg);
   // void print() const;
};


#endif //CFG_LLPARSER1_H
