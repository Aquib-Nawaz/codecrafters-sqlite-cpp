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

void skipColumnValues(std::ifstream *is, const std::vector<uint64_t> &types,
                             int columnNo ){

    for(int i = 0; i < columnNo ; i++){
        uint64_t type = types[i];
        is->seekg(getNumBytes(type), std::ios_base::cur);
    }
}

template<>
char *getColumn(std::ifstream *is, uint64_t type){
    char* selectColumnValue;
    if(type>=13 && type%2==1){
        type = COLUMN_VALUE_STRING_SIZE(type);
        selectColumnValue = (char *) malloc(type + 1);
        selectColumnValue[type] = '\0';
        is->read(selectColumnValue, (long)type);
    }
    else{
        selectColumnValue = (char *) malloc(1) ;
        selectColumnValue[0]='\0';
    }
    return selectColumnValue;
}

template<>
uint64_t getColumn(std::ifstream *is, uint64_t type) {
    if(type==7 || type>9){
        std::cout << type << std::endl;
        assert(0);
    }
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
//    int64_t fileOffset = (int64_t)(pageSize)*(pageNum-1);
//    if(pageNum==1){
//        //Skip DB Header
//        fileOffset += HEADER_SIZE;
//    }
//    is->seekg(fileOffset+BTREE_TYPE_OFFSET_1);
//    char c;
//    is->read(&c, 1);
    uint64_t ret = 0;
//
//    is->seekg(fileOffset + NUMBER_OF_CELLS_OFFSET_2);
//    uint16_t numCell = bigEndian(is, 2);
    Page page(is, pageNum, pageSize, false);
    switch (page.pageType) {
        case LEAF_TABLE_B_TREE_TYPE:
        case LEAF_INDEX_B_TREE_TYPE:
            ret = page.numCell;
            break;
        case INTERIOR_INDEX_BTREE_TYPE:
        case INTERIOR_TABLE_BTREE_TYPE:{
            uint32_t lastPageNum = page.lastPageNum;

            for (int cell=0; cell<page.numCell; cell++){
                is->seekg(page.cellsOffset[cell]);
                uint32_t childPageNum = bigEndian(is,4);
                ret += countRows(is, childPageNum, pageSize);
            }
            ret += countRows(is, lastPageNum, pageSize);
            break;

        }

    }
    return ret;
}

static uint64_t getColumnType(std:: ifstream* is, int retColumnNum, uint64_t payloadBodyOffset, const std::vector<uint64_t>& types){
    is->seekg(payloadBodyOffset);
    skipColumnValues(is, types, retColumnNum - 1);

    return types[retColumnNum - 1];
}

template<>
void populateReturnColumnsList<uint64_t , int>(std:: ifstream* is, const int& retColumnNum , std::vector<uint64_t >* returnList,
                                               uint64_t payloadBodyOffset, const std::vector<uint64_t>& types, uint64_t ){
    if(returnList!= nullptr) {
        uint64_t type = getColumnType(is, retColumnNum, payloadBodyOffset, types);
        returnList->push_back(getColumn<uint64_t>(is, type));

    }
}

template<>
void populateReturnColumnsList<char* , int>(std:: ifstream* is, const int& retColumnNum , std::vector<char* >* returnList,
                              uint64_t payloadBodyOffset, const std::vector<uint64_t>& types, uint64_t){
    if(returnList!= nullptr) {
        uint64_t type = getColumnType(is, retColumnNum, payloadBodyOffset, types);
        returnList->push_back(getColumn<char*>(is, type));

    }
}

template<>
void populateReturnColumnsList<std::string , int>(std:: ifstream* is, const int& retColumnNum , std::vector<std::string >* returnList,
                              uint64_t payloadBodyOffset, const std::vector<uint64_t>& types, uint64_t ){
    if(returnList!= nullptr) {
        uint64_t type = getColumnType(is, retColumnNum, payloadBodyOffset, types);
        returnList->push_back(getColumn<std::string>(is, type));
    }
}

template<>
void populateReturnColumnsList<std::string , std::vector<int>>(std:: ifstream* is, const std::vector<int> &retColumnNums
        , std::vector<std::string >* returnList,
        uint64_t payloadBodyOffset, const std::vector<uint64_t>& types, uint64_t id){
    if(returnList!= nullptr) {
        std::string toWrite = "";
        int i;
        for(i=0; i<retColumnNums.size()-1; i++){
            int retColumnNum = retColumnNums[i];
            if(retColumnNum==1 && types[0]==0){
                toWrite+=std::to_string(id);
            }
            else{
                uint64_t type = getColumnType(is, retColumnNum, payloadBodyOffset, types);
                toWrite += getColumn<std::string>(is, type);
            }
            toWrite+="|";
        }
        if(retColumnNums[i]==1 && types[0] == 0)
            toWrite += std::to_string(id);
        else
            toWrite += getColumn<std::string>(is, getColumnType(is, retColumnNums[i],
                                                                payloadBodyOffset, types));
        returnList->push_back(toWrite);
    }
}

void searchByRowId(std::ifstream *is, int pageNum, const std::vector<uint64_t>&rowIds, int pageSize, int & currRowIdIdx,
                   const std::vector<int> & retColumnNum , std::vector<std::string>* returnList){
    Page page(is, pageNum, pageSize);
    switch(page.pageType) {
        case LEAF_TABLE_B_TREE_TYPE: {
            for (int cell=0; cell<page.numCell; cell++){
                is->seekg(page.cellsOffset[cell]);
                bigEndianVarInt(is);
                rowId_t key = bigEndianVarInt(is);
                uint64_t payloadStartOffset = is->tellg();

                uint64_t payloadHeaderSize = bigEndianVarInt(is);
                if(currRowIdIdx==rowIds.size())
                    return;
                if(key!=rowIds[currRowIdIdx]){
                    continue;
                }
                currRowIdIdx++;
                std::vector<uint64_t> types;
                for (int i = 0; i < max(1, getMax(retColumnNum)) ; i++) {
                    types.push_back(bigEndianVarInt(is));
                }
                populateReturnColumnsList(is, retColumnNum, returnList, payloadStartOffset+
                    payloadHeaderSize, types, key);

            }
            break;
        }
        case INTERIOR_TABLE_BTREE_TYPE: {
            uint32_t lastPageNum = page.lastPageNum;
            int cell;
            rowId_t key;
            for (cell=0; cell<page.numCell; cell++){
                is->seekg(page.cellsOffset[cell]);
                uint64_t childPageNum = bigEndian(is,4);
                key = bigEndianVarInt(is);
                if(currRowIdIdx==rowIds.size())
                    return;
                if(key >= rowIds[currRowIdIdx]){
                    searchByRowId(is,childPageNum, rowIds,pageSize, currRowIdIdx, retColumnNum,
                                         returnList);
                    if(key > rowIds[currRowIdIdx])
                        break;
                }
            }
            if(cell==page.numCell || (rowIds.size()!=currRowIdIdx && key == rowIds[currRowIdIdx]) )
                searchByRowId(is,lastPageNum, rowIds,pageSize, currRowIdIdx, retColumnNum,
                              returnList);
            break;
        }
        default:
            std::cout << "Unsupported Pagetype " << page.pageType << std::endl;
    }
}

#if 0
int main(){
  auto splitted = split("id integer primary key, chocolate text,coffee text,pistachio text,strawberry text,grape text", ",");
  for(auto col:splitted){
      std::cout << trim(col) << std::endl;
  }
}
#endif
