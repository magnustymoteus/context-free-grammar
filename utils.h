//

#ifndef CFG_UTILS_H
#define CFG_UTILS_H

#endif //CFG_UTILS_H

#include <set>
#include <string>
#include <map>

void printSet(const std::set<std::string> &set);
void printSets(const std::map<std::string, std::set<std::string>> &sets);
void insertIfNotASubset(std::set<std::string> &a, const std::set<std::string> &b, bool &hasChanged);