//
// Created by gruzi on 30/09/2023.
//
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "json.hpp"

#include "CFG.h"

using nlohmann::json;

bool hasConflicts(const std::set<std::string> &variables, const std::set<std::string> &terminals) {
    for(const std::string &currentVariable : variables) {
        if(terminals.find(currentVariable) != terminals.end()) return true;
    }
    return false;
}

CFG::CFG(const std::set<std::string> &variables_arg,
         const std::set<std::string> &terminals_arg,
         std::map<std::string, CFGProductionBodies> &production_rules_arg,
         const std::string &starting_variable_arg) :

         variables(variables_arg), terminals(terminals_arg), production_rules(production_rules_arg),
         starting_variable(starting_variable_arg){

    std::string errorMessage;
    if(!isValid(errorMessage)) throw std::invalid_argument(errorMessage.c_str());
}
CFG::CFG(const std::string &jsonPath) {
    std::ifstream input(jsonPath);
    json j = json::parse(input);

    variables = j["Variables"];
    terminals = j["Terminals"];
    const json &productions = j["Productions"];
    for(const json &currentProduction : productions) {
        const std::string &head = currentProduction["head"];
        CFGProductionBody body = currentProduction["body"];
        if(body.empty()) body.insert(body.end(), "");
        production_rules[head].insert(production_rules[head].end(), body);
    }
    starting_variable = j["Start"];


    std::string errorMessage;
    if(!isValid(errorMessage)) throw std::invalid_argument(errorMessage.c_str());
}
bool CFG::isValid(std::string &errorMessageRef) const {
    std::string errorMessage;
    if(variables.find(starting_variable) == variables.end()) {
        errorMessage = "Starting variable is not a member of the variable set";
    }
    if(hasConflicts(variables, terminals)) {
        errorMessage="Intersection of variables and terminals is not empty!";
    }
    for(const std::pair<std::string, CFGProductionBodies> currentRule : production_rules) {
        if(variables.find(currentRule.first) == variables.end()) {
            errorMessage = "Head of a rule is not in variables set!";
        }
        for(const CFGProductionBody &currentBody : currentRule.second) {
            for(const std::string &currentString : currentBody) {
                if(!currentString.empty() && (variables.find(currentString) == variables.end() &&
                   terminals.find(currentString) == terminals.end())) {
                    errorMessage = "A string in rule body contains an invalid symbol '"+currentString+"' !";
                }
            }
        }
    }
    if(!errorMessage.empty()) errorMessageRef = errorMessage;
    return errorMessage.empty();
}

void CFG::print() const {
    std::cout << "V = {";
    for (const std::string &currentVariable: variables) {
        std::cout << currentVariable;
        if (*(--variables.end()) != currentVariable) std::cout << ", ";
    }
    std::cout << "}\n";

    std::cout << "T = {";
    for (const std::string &currentTerminal: terminals) {
        std::cout << currentTerminal;
        if (*(--terminals.end()) != currentTerminal) std::cout << ", ";
    }
    std::cout << "}\n";

    std::cout << "P = {\n";
    for (std::pair<std::string, CFGProductionBodies> currentRule: production_rules) {
        sort(currentRule.second.begin(), currentRule.second.end());
        for (int i=0; i < currentRule.second.size(); i++) {
            std::cout << "\t" << currentRule.first << " -> `";
            for (int j=0; j < currentRule.second[i].size(); j++) {
                std::cout << currentRule.second[i][j];
                if(j+1 != currentRule.second[i].size()) std::cout << " ";
            }
            std::cout << "`\n";
        }
    }
    std::cout << "}\n";
    std::cout << "S = " << starting_variable;
}
