//

#include "AugmentedCFG.h"
#include "utils.h"
#include <iostream>

using namespace CFGUtils;

CFG AugmentedCFG::constructCFGFromItemSet(const ItemSet &itemSet, const std::string &starting) const {
    CFGProductionRules newProductionRules;
    for(const auto& currentProductions : itemSet) {
        std::vector<std::vector<std::string>> CFGBodies;
        for(const auto& currentBody : currentProductions.second.getBodies()){
            const std::vector<std::string> &content = currentBody.getContent();
            const std::vector<std::string> newBody (content.begin()+currentBody.getReadingIndex(), content.end());
            CFGBodies.push_back(newBody);
        }
        newProductionRules.insert({currentProductions.first, CFGBodies});
    }
    return CFG{grammar.getVariables(), grammar.getTerminals(), newProductionRules, starting};
}

ItemSet AugmentedCFG::computeClosure(const std::pair<std::string, AugmentedProductionBody> &production,
                                     const ItemSet &availableProductions) const {
    ItemSet result;
    const std::set<std::string> &lookaheads = availableProductions.at(production.first).lookaheads;
    result.insert({production.first, AugmentedProductions({production.second}, lookaheads)});
    const unsigned int readingIndex = production.second.getReadingIndex();

    if(readingIndex < production.second.getContent().size()) {

        const std::string readSymbol = production.second.getContent()[production.second.getReadingIndex()];
        if(!grammar.isTerminal(readSymbol)) {

            result.insert({readSymbol, availableProductions.at(readSymbol)});

            for(const auto& productions : result) {
                for(const auto& currentBody : productions.second.getBodies()) {
                    const std::string readingSymbol = currentBody.getContent()[currentBody.getReadingIndex()];
                    if(result.find(readingSymbol) == result.end()) {
                        const ItemSet &newItemSet =
                                computeClosure({productions.first, currentBody}, availableProductions);
                        result.insert(newItemSet.begin(), newItemSet.end());
                    }
                }
            }

        }
    }

    return result;
}
bool AugmentedProductionBody::operator<(const AugmentedProductionBody &body) const {
    return getContent() < body.getContent();
}
bool AugmentedProductions::operator<(const AugmentedProductions &productions) const {
    return getBodies() < productions.getBodies();
}
void AugmentedCFG::computeNextItemSets(const ItemSet &itemSet) {
    for(const auto& currentProductions : itemSet) {
        for(const auto& currentBody : currentProductions.second.getBodies()) {
            if(currentBody.getReadingIndex() < currentBody.getContent().size()) {
                const std::string nextHead = currentBody.getContent()[currentBody.getReadingIndex()];
                ItemSet nextItemSet = computeClosure({currentProductions.first, currentBody}, startingItemSet);
                const CFG &cfg = constructCFGFromItemSet(nextItemSet, currentProductions.first);
                const std::map<std::string, std::set<std::string>> & followSet = cfg.getFollowSets();
                for(auto& currentItem : nextItemSet) {
                    currentItem.second.lookaheads = followSet.at(currentItem.first);
                }
                const bool domainIsNewInMap = itemSetTransitionMap.find(itemSet) == itemSetTransitionMap.end();
                if(domainIsNewInMap || itemSetTransitionMap.find(itemSet)->second.find(nextHead) ==
                                       itemSetTransitionMap.find(itemSet)->second.end()) {
                    itemSetTransitionMap[itemSet][nextHead] = nextItemSet;
                    computeNextItemSets(nextItemSet);
                }
            }
        }
    }
}
AugmentedCFG::AugmentedCFG(const CFG &cfg) : startingVariable(cfg.getStartingVariable()+"'"), grammar(cfg) {
    // add S' manually
    AugmentedProductionBody startingProductionBody({cfg.getStartingVariable()});
    AugmentedProductions startingProduction({startingProductionBody}, {EOS_MARKER});
    startingItemSet.insert({getStartingVariable(), startingProduction});
    const std::map<std::string, std::set<std::string>> &followSets = cfg.getFollowSets();

    for(const auto &currentProductions : cfg.getProductionRules()) {
    std::vector<AugmentedProductionBody> bodies;
         for(const auto &currentBody : currentProductions.second) {
            AugmentedProductionBody body(currentBody);
            bodies.push_back(body);
        }
         AugmentedProductions productions(bodies, followSets.at(currentProductions.first));
         startingItemSet.insert({currentProductions.first, productions});
    }

    //compute item sets
    startingItemSet = computeClosure({startingVariable, startingProductionBody}, startingItemSet);
    computeNextItemSets(startingItemSet);
}

void AugmentedCFG::print() const {
    for(const auto& currentProductions : getStartingItemSet()) {
        for(const auto& currentBody : currentProductions.second.getBodies()) {
            std::cout << currentProductions.first << " -> `";
            for(unsigned int i=0;i<currentBody.getContent().size();i++) {
                if(currentBody.getReadingIndex() == i) std::cout << ".";
                std::cout << currentBody.getContent()[i];
                if(i < currentBody.getContent().size()-1) std::cout << " ";
            }
            if(currentBody.getReadingIndex() == currentBody.getContent().size()) std::cout << ".";
            std::cout << "`, ";
            printSet(currentProductions.second.getLookaheads());
        }
    }
}

std::string AugmentedCFG::getStartingVariable() const {
    return startingVariable;
}

ItemSet AugmentedCFG::getStartingItemSet() const {
    return startingItemSet;
}
