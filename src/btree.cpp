//
// Created by Aquib Nawaz on 15/02/24.
//
#include "btree.h"

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

#if 0
int main(){
  auto splitted = split("id integer primary key, chocolate text,coffee text,pistachio text,strawberry text,grape text", ",");
  for(auto col:splitted){
      std::cout << trim(col) << std::endl;
  }
}
#endif
