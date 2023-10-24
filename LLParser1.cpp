//
// Created by gruzi on 22/10/2023.
//

#include "LLParser1.h"

#include "CFG.h"

#include <iostream>


LL1Parser::LL1Parser(const CFG &cfg) : firstSets(cfg.getAllFirstSets()), followSets(cfg.getAllFollowSets()), cfg(cfg)
{}

void printSet(const std::set<std::string> &set) {
    std::string result = "{";
    for(const std::string &currentSymbol : set) {
        result += currentSymbol+", ";
    }
    std::cout << result.substr(0, result.size()-2)+"}" << std::endl;
}
void printSets(const std::map<std::string, std::set<std::string>> &sets) {
    if(sets.size() == 1) printSet(sets.begin()->second);
    else {
        std::cout << std::endl;
        for(const auto & currentVariable : sets) {
            std::cout << "\t" << currentVariable.first << ": ";
            printSet(currentVariable.second);
        }
    }
}
std::set<std::string> getTerminalsWithEOS(const std::set<std::string> &terminals) {
    std::set<std::string> newTerminals = terminals;
    newTerminals.insert("<EOS>");
    return newTerminals;
}
std::string getColumnStr(const std::string &symbol, const unsigned int &space) {
    std::string str(space+3, ' ');
    str.replace(1, symbol.length(), symbol);
    return "|"+str;
}
LL1ParsingTable LL1Parser::getParsingTable() const {

    LL1ParsingTable parsingTable; // row : (column : value)
    for(const std::string &currentVariable : cfg.getVariables()) {
        for(const std::string &currentTerminal : getTerminalsWithEOS(cfg.getTerminals())) {
            parsingTable[currentVariable][currentTerminal] = "<ERR>";
        }
    }
    for(const std::string &currentVariable : cfg.getVariables()) {
        for(const CFGProductionBody &currentBody : cfg.getProductionBodies(currentVariable)) {
            std::set<std::string> firstSet = {currentBody[0]};
            if(!cfg.isTerminal(currentBody[0]) && !currentBody[0].empty()) firstSet = firstSets.at(currentBody[0]);
            else if(currentBody[0].empty()) {
                // to do
            }
            for(const std::string &currentTerminal : firstSet) {
                parsingTable[currentVariable][currentTerminal] = CFG::bodyToStr(currentBody);
            }
        }
    }
    return parsingTable;
}

std::map<std::string, unsigned int> LL1Parser::getColumnWidthsOfParsingTable(const LL1ParsingTable &parsingTable) const
{
    std::map<std::string, unsigned int> columnWidths;

    for(const std::string &currentVariable : cfg.getVariables()) {
        for(const std::string &currentTerminal : getTerminalsWithEOS(cfg.getTerminals())) {
            const unsigned int &suggestedLength = parsingTable.at(currentVariable).at(currentTerminal).length();
            if(columnWidths[currentTerminal] < suggestedLength) {
                columnWidths[currentTerminal] = suggestedLength;
            }
        }
    }
    return columnWidths;
}
void LL1Parser::printHorizontalLine(const std::map<std::string,
                         unsigned int> &columnWidths, const unsigned int &variableColumnWidth) const {
    std::cout << "|" << std::string(variableColumnWidth, '-') << "|";
    for(const std::string &currentTerminal : getTerminalsWithEOS(cfg.getTerminals())) {
        std::cout << std::string(columnWidths.at(currentTerminal)+3, '-') << "|";
    }
    std::cout << std::endl;
}
void LL1Parser::printParsingTable(const LL1ParsingTable &parsingTable,
                                  const std::map<std::string, unsigned int> &columnWidths) const {
    const unsigned int variableColumnWidth = 4;
    const std::set<std::string> & terminals = getTerminalsWithEOS(cfg.getTerminals());
    std::cout << getColumnStr("", 1);
    for(const std::string &currentTerminal : terminals) {
        std::cout << getColumnStr(currentTerminal, columnWidths.at(currentTerminal));
    }
    std::cout << "|";
    std::cout << std::endl;
    printHorizontalLine(columnWidths, variableColumnWidth);
    for(const std::string &currentVariable : cfg.getVariables()) {
        std::cout << getColumnStr(currentVariable, 1);
        for(const std::string &currentTerminal : terminals) {
            std::cout << getColumnStr(parsingTable.at(currentVariable).at(currentTerminal), columnWidths.at(currentTerminal));
        }
        std::cout << "|";
        std::cout << std::endl;
    }
    printHorizontalLine(columnWidths, variableColumnWidth);
}
void LL1Parser::print() const {
    std::cout << ">>> Building LL(1) Table\n";
    const LL1ParsingTable & parsingTable = getParsingTable();
    const std::map<std::string, unsigned int> &columnWidths = getColumnWidthsOfParsingTable(parsingTable);


    std::cout << " >> FIRST: ";
    printSets(firstSets);

    std::cout << " >> FOLLOW: ";
    printSets(followSets);

    std::cout << ">>> Table is built.\n\n-------------------------------------\n\n";
    printParsingTable(parsingTable, columnWidths);
}
