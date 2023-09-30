#include <iostream>
#include "CFG.h"

using namespace std;
// to do, adjust code to the multimap (both the task 1 and 2)
int main() {
    CFG cfg("CFG.json");
    cfg.print();
    return 0;
}