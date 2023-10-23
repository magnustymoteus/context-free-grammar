//
// Created by gruzi on 30/09/2023.
//
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "json.hpp"

#include "CFG.h"
#include "LLParser1.h"

using nlohmann::json;


bool hasConflicts(const std::set<std::string> &variables, const std::set<std::string> &terminals) {
    for(const std::string &currentVariable : variables) {
        if(terminals.find(currentVariable) != terminals.end()) return true;
    }
    return false;
}
bool CFG::isTerminal(const std::string &symbol) const {
    return terminals.find(symbol) != terminals.end();
}

void CFG::ll() const {
    LL1Parser(*this);
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

std::set<std::string> CFG::getVariables() const {return variables;}
std::set<std::string> CFG::getTerminals() const {return terminals;}
CFGProductionRules  CFG::getProductionRules() const {return production_rules;}
std::string CFG::getStartingVariable() const {return starting_variable;}
CFGProductionBodies CFG::getProductionBodies(const std::string &productionHead) const {
    return production_rules.at(productionHead);
}

std::set<std::string> CFG::getFirstSet(const std::string &variable) const {
    std::set<std::string> result;
    if(isTerminal(variable) || variable.empty())
        return {variable};
    for(const CFGProductionBody &currentBody : getProductionBodies(variable)) {
        const std::set<std::string> &set = getFirstSet(currentBody[0]);
        if(set.find("") != set.end() && currentBody.size() > 1) {
            const std::set<std::string> &nextSet = getFirstSet(currentBody[1]);
            result.insert(nextSet.begin(), nextSet.end());
        }
        result.insert(set.begin(), set.end());
    }
    return result;
}

std::map<std::string, std::set<std::string>> CFG::getAllFirstSets() const {
    std::map<std::string, std::set<std::string>> result;
    for(const std::string &currentVariable : getVariables()) {
        result[currentVariable] = getFirstSet(currentVariable);
    }
    return result;
}

void insertIfNotASubset(std::set<std::string> &a, const std::set<std::string> &b, bool &hasChanged) {
    const size_t sizeBefore = a.size();
    a.insert(b.begin(), b.end());
    hasChanged = hasChanged || sizeBefore != a.size();
}
void CFG::setFollowSet(const std::string &variable,
                                        std::map<std::string,std::set<std::string>> &followSets,
                                        bool &setHasChanged) const {
    // TO DO: fix this by replacing FIRST(...) - EPSILON U FOLLOW(...) TO FIRST(...) - EPSILOJ U FIRST(...) because
    // when epsilon gets derived u need to get the first of the next one

    /* FOLLOW(A) = {a|S ⇒* αAaβ where α, β can be any strings} */
    std::cout << "--START PRODUCTIONS OF " << variable << "--\n";
    if(variable == getStartingVariable())
        insertIfNotASubset(followSets[variable],{"<EOS>"}, setHasChanged);
    for(const CFGProductionBody &currentBody : getProductionBodies(variable)) {
        for(int i=0;i<currentBody.size();i++) {
            const std::string &current = currentBody[i];
            const bool &currentIsTerminal = isTerminal(current);
            /* If production is of form A → α B β, β ≠ ε. */
            if(i != currentBody.size()-1) {
                const std::string &next = currentBody[i+1];
                if(!currentIsTerminal) {
                    std::set<std::string> firstSet = getFirstSet(next);
                    /* (b) If FIRST (β) contains ε (i. e. , β ⇒* ε), then FOLLOW (B) = FIRST (β) − {ε} ∪ FOLLOW (A) */
                    std::cout << "FOLLOW(" << current << ") = FIRST(" << next << ")";
                    if(firstSet.find("") != firstSet.end()) {
                        firstSet.erase("");
                        const std::set<std::string> &followSet = followSets[variable];
                        std::cout << " U FOLLOW(" << variable << ")";
                        insertIfNotASubset(followSets[current], followSet, setHasChanged);
                    }
                    std::cout << std::endl;
                    /* (a) If FIRST (β) does not contain ε then, FOLLOW (B) = {FIRST (β)} */
                    insertIfNotASubset(followSets[current], firstSet, setHasChanged);
                }
            }
            /* If production is of form A → αB, then Follow (B) ={FOLLOW (A)}. */
            else if(!currentIsTerminal && !current.empty()) {
                const std::set<std::string> &followSet = followSets[variable];
                std::cout << "FOLLOW(" << current << ") = FOLLOW(" << variable << ")\n";
                insertIfNotASubset(followSets[current], followSet, setHasChanged);
            }
        }
    }
    std::cout << "--END PRODUCTIONS OF " << variable << "--\n\n";
}




std::map<std::string, std::set<std::string>> CFG::getAllFollowSets() const {
    std::map<std::string, std::set<std::string>> result;
    bool setHasChanged = true;
    while(setHasChanged) {
        setHasChanged = false;
        for (const std::string &currentVariable: getVariables()) {
            setFollowSet(currentVariable, result, setHasChanged);
        }
        std::cout << std::endl;
    }
    return result;
}