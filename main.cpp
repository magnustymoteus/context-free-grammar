#include "CFG.h"
#include "AugmentedCFG.h"
#include "utils.h"


using namespace std;

int main() {
    CFG cfg("input/CFG.json");
    AugmentedCFG aCfg(cfg);
    cfg.print();
    aCfg.print();
    return 0;
}