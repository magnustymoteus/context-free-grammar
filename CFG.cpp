//
// Created by gruzi on 30/09/2023.
//
#include <stdexcept>
#include <iostream>

#include "CFG.h"
bool hasConflicts(const std::set<std::string> &variables, const std::set<std::string> &terminals) {
    for(const std::string &currentVariable : variables) {
        if(terminals.find(currentVariable) != terminals.end()) return true;
    }
    return false;
}

CFG::CFG(const std::set<std::string> &variables_arg,
         const std::set<std::string> &terminals_arg,
         std::map<std::string, std::list<std::list<std::string>>> &production_rules_arg,
         const std::string &starting_variable_arg) :

         variables(variables_arg), terminals(terminals_arg), production_rules(production_rules_arg),
         starting_variable(starting_variable_arg){

    std::string errorMessage;
    if(!isValid(errorMessage)) throw std::invalid_argument(errorMessage.c_str());
}

CFG::CFG() {
    //hardcoded for now
    std::set<std::string> new_variables = {"BINDIGIT", "S"};
    std::set<std::string> new_terminals = {"0", "1", "a", "b"};
    std::map<std::string, std::list<std::list<std::string>>> new_production_rules =
            {{"BINDIGIT", {{"0"}, {"1"}}}, {"S", {{""}, {"a", "S", "b", "BINDIGIT"}}}};
    std::string new_starting_variable = "S";

    variables = new_variables;
    terminals = new_terminals;
    production_rules = new_production_rules;
    starting_variable = new_starting_variable;

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
    for(const std::pair<std::string, std::list<std::list<std::string>>> currentRule : production_rules) {
        if(variables.find(currentRule.first) == variables.end()) {
            errorMessage = "Head of a rule is not in variables set!";
        }
        for(const std::list<std::string> &currentBody : currentRule.second) {
            for(const std::string &currentString : currentBody) {
                if(currentString != "" && (variables.find(currentString) == variables.end() &&
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
    for (const std::pair<std::string, std::list<std::list<std::string>>> currentRule: production_rules) {
        for (const std::list<std::string> &currentBody: currentRule.second) {
            std::cout << "\t" << currentRule.first << " -> `";
            for (const std::string &currentString: currentBody) {
                std::cout << currentString;
                if(*(--currentBody.end()) != currentString) std::cout << " ";
            }
            std::cout << "`\n";
        }
    }
    std::cout << "}\n";
    std::cout << "S = " << starting_variable;
}
