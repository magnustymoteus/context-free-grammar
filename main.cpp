#include "CFG.h"

using namespace std;

int main() {
    CFG cfg("input-ll1.json");
    cfg.ll().print();
    return 0;
}