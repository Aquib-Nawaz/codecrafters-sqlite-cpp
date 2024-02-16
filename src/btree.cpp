//
// Created by Aquib Nawaz on 15/02/24.
//
#include "btree.h"

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

uint64_t getNumBytes(uint64_t type){
    if(type<=4)
        return type;

    else if(type==5)
        return 6;

    else if(type<=7)
        return 8;

    else if(type<=9)
        return 0;

    else if(type>=12 && type%2==0)
        return COLUMN_VALUE_BLOB_SIZE(type);

    assert(type>=13 && type%2==1);
    return COLUMN_VALUE_STRING_SIZE(type);
}

void skipColumnValues(std::ifstream *is, std::vector<uint64_t> &types,
                             int columnNo ){

    for(int i = 0; i < columnNo ; i++){
        uint64_t type = types[i];
        is->seekg(getNumBytes(type), std::ios_base::cur);
    }
}

template<>
char *getColumn(std::ifstream *is, uint64_t type){

    char* selectColumnValue;
    type = COLUMN_VALUE_STRING_SIZE(type);
    selectColumnValue = (char *) malloc(type + 1);
    selectColumnValue[type] = '\0';
    is->read(selectColumnValue, (long)type);
    return selectColumnValue;
}

template<>
uint64_t getColumn(std::ifstream *is, uint64_t type) {
    assert(type>0 && type<=6);
    return bigEndian(is, (int)getNumBytes(type));
}

template<>
std::string getColumn(std::ifstream *is, uint64_t type){
    std::string selectColumnValue;
    if(type<12)
        selectColumnValue = std::to_string(getColumn<uint64_t>(is,type));
    else{
        char* temp = getColumn<char*>(is, type);
        selectColumnValue = temp;
        free(temp);
    }
    return selectColumnValue;
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

uint64_t countRows(std::ifstream *is, int pageNum, int pageSize){
    int64_t fileOffset = (int64_t)(pageSize)*(pageNum-1);
    if(pageNum==1){
        //Skip DB Header
        fileOffset += HEADER_SIZE;
    }
    is->seekg(fileOffset+BTREE_TYPE_OFFSET_1);
    char c;
    is->read(&c, 1);
    uint64_t ret = 0;

    is->seekg(fileOffset + NUMBER_OF_CELLS_OFFSET_2);
    uint16_t numCell = bigEndian(is, 2);
    switch (c) {
        case LEAF_TABLE_B_TREE_TYPE:
        case LEAF_INDEX_B_TREE_TYPE:
            ret = numCell;
            break;
        case INTERIOR_TABLE_BTREE_TYPE:
        case INTERIOR_INDEX_BTREE_TYPE:
            printf("Unsupported Page Type\n");
    }
    return ret;
}

void toLower(std::string data) {
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}

#if 0
int main(){
    std::ifstream database_file("sample.db", std::ios::binary);
    database_file.seekg(16);  // Skip the first 16 bytes of the header
//    std::cout << bigEndian(&database_file, 2) << std::endl;
    std::string str(10, ' ');
    database_file.read(str.data(), 10);
}
#endif
