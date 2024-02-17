//
// Created by Aquib Nawaz on 17/02/24.
//

#ifndef GIT_STARTER_CPP_UTILITY_H
#define GIT_STARTER_CPP_UTILITY_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <cctype>


std::string trim(const std::string &, const std::string & delimeters = ", \t\n;");
void toLower(std::string &);
std::vector<std::string> split(const std::string& s, const std::string& delimiter);
uint64_t bigEndianVarInt(std::ifstream* , int maxLength=9);
uint64_t bigEndian(std::ifstream* , int length);

template <typename T>
int getMax(T);


#endif //GIT_STARTER_CPP_UTILITY_H
