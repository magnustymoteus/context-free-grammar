//
// Created by gruzi on 22/10/2023.
//

#include "LLParser1.h"
#include <iostream>


LL1Parser::LL1Parser(const CFG &cfg) {
    firstSet = cfg.getAllFirstSets();
    followSet = cfg.getAllFollowSets();
    for(const auto &currentFirstSet : firstSet) {
        std::cout << "FIRST(" << currentFirstSet.first << ") = {";
        for(const std::string &firstSetElement : currentFirstSet.second) {
            std::cout << firstSetElement;
            if(firstSetElement != *currentFirstSet.second.rbegin()) std::cout << ",";
        }
        std::cout << "}\n";
    }
    std::cout << "\n";
    for(const auto &currentFollowSet : followSet) {
        std::cout << "FOLLOW(" << currentFollowSet.first << ") = {";
        for(const std::string &followSetElement : currentFollowSet.second) {
            std::cout << followSetElement;
            if(followSetElement != *currentFollowSet.second.rbegin()) std::cout << ",";
        }
        std::cout << "}\n";
    }
}