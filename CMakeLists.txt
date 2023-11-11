cmake_minimum_required(VERSION 3.22)
project(CFG)

set(CMAKE_CXX_STANDARD 17)

add_executable(CFG main.cpp CFG.cpp LL1Parser.cpp  CLR1Parser.cpp utils.cpp AugmentedCFG.cpp AugmentedCFG.cpp)
