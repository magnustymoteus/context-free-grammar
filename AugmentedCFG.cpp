//

#include "AugmentedCFG.h"
#include "utils.h"
#include <iostream>


AugmentedCFG::AugmentedCFG(const CFG &cfg) : startingVariable(cfg.getStartingVariable()+"'") {
    // add S' manually
    AugmentedProductionBody startingProductionBody({cfg.getStartingVariable()});
    AugmentedProductions startingProduction({startingProductionBody}, {EOS_MARKER});
    startingItemSet.insert({getStartingVariable(), startingProduction});
    const std::map<std::string, std::set<std::string>> &followSets = cfg.getFollowSets();

    // initialize augmented productions before computing lookaheads
    for(const auto &currentProductions : cfg.getProductionRules()) {
    std::vector<AugmentedProductionBody> bodies;
         for(const auto &currentBody : currentProductions.second) {
            AugmentedProductionBody body(currentBody);
            bodies.push_back(body);
        }
         AugmentedProductions productions(bodies, followSets.at(currentProductions.first));
         startingItemSet.insert({currentProductions.first, productions});
    }

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
