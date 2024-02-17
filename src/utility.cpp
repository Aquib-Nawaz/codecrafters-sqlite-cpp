//
// Created by Aquib Nawaz on 17/02/24.
//

#include "utility.h"

std::string trim(const std::string &str){
    std::string delimeters = ", \t\n;";
    int i;
    for(i=0; i<str.size(); i++){
        if(delimeters.find(str[i])==std::string::npos)
            break;
    }
    int j;
    for(j=str.size()-1; j>=i; j--){
        if(delimeters.find(str[j])==std::string::npos)
            break;
    }
    return str.substr(i, j-i+1);
}

void toLower(std::string &data) {
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if(!token.empty())
            res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}


uint64_t bigEndianVarInt(std::ifstream *is, int maxLength){
    char c;
    is->read(&c, 1);
    unsigned char d = static_cast<unsigned char>(c);

    uint64_t ret = 0;
    int length =1;
    while((d>>7 & 1) && length<maxLength){
        ret = ret << 7 | d & (1<<7)-1;
        is->read(&c, 1);
        d = static_cast<unsigned char>(c);
        length ++;
    }
    ret = ret << 7 | d;
    return ret;
}

uint64_t bigEndian(std::ifstream *is, int length){
    char c;
    is->read(&c, 1);
    unsigned char d = static_cast<unsigned char>(c);
    int len = 1;
    uint64_t ret=0;
    while( len<=length){
        ret = ret << 8 | d ;
        is->read(&c, 1);
        d = static_cast<unsigned char>(c);
        len ++;
    }
    return ret;
}

template<>
int getMax<int>(int v){
    return v;
}

template<>
int getMax<std::vector<int>>(std::vector<int> v){
    return *max_element(v.begin(), v.end());
}

#if 0
int main(){
    std::string word = "COUNT";
    toLower(word);
    std::cout << word << std::endl;
}
#endif