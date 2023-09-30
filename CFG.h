//
// Created by gruzi on 30/09/2023.
//

#ifndef CFG_CFG_H
#define CFG_CFG_H

#include <set>
#include <list>
#include <map>
#include <string>

class CFG {
public:
    std::set<std::string> variables;
    std::set<std::string> terminals;
    std::multimap<std::string, std::list<std::string>> production_rules;
    std::string starting_variable;

    CFG(const std::set<std::string> &variables_arg,
        const std::set<std::string> &terminals_arg,
        std::map<std::string, std::list<std::list<std::string>>> &production_rules_arg,
        const std::string &starting_variable_arg);
    CFG(const std::string &jsonPath);

    bool isValid(std::string &errorMessage) const;
    void print() const;
};


#endif //CFG_CFG_H
