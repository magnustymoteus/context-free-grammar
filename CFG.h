//
// Created by gruzi on 30/09/2023.
//

#ifndef CFG_CFG_H
#define CFG_CFG_H

#include <set>
#include <vector>
#include <map>
#include <string>


typedef std::vector<std::string> CFGProductionBody;
typedef std::vector<CFGProductionBody> CFGProductionBodies;

class CFG {
public:
    std::set<std::string> variables;
    std::set<std::string> terminals;
    std::map<std::string, CFGProductionBodies> production_rules; // key: head : array of bodies
    std::string starting_variable;

    CFG(const std::set<std::string> &variables_arg,
        const std::set<std::string> &terminals_arg,
        std::map<std::string, std::vector<std::vector<std::string>>> &production_rules_arg,
        const std::string &starting_variable_arg);
    CFG() = default;
    explicit CFG(const std::string &jsonPath);

    bool isValid(std::string &errorMessage) const;
    void print() const;
};


#endif //CFG_CFG_H
