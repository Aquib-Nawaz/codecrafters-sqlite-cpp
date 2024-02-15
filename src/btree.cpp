//
// Created by Aquib Nawaz on 15/02/24.
//
#include "btree.h"
#include <cassert>
#include <vector>
#include <cstring>

uint64_t bigEndianVarInt(std::ifstream *is, int maxLength){
    unsigned char d;
    *is >> d;

    uint64_t ret = 0;
    int length =1;
    while((d>>7 & 1) && length<maxLength){
        ret = ret << 7 | d & (1<<7)-1;
        *is >> d;
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

static void skipColumnValues(std::ifstream *is, std::vector<uint64_t> &types){
    int columnNo =  types.size()-1;
    for(int i = 0; i < columnNo ; i++){
        uint64_t type = types[i];
        switch (type) {
            case COLUMN_VALUE_NULL_TYPE:
                break;
            case COLUMN_VALUE_1_BYTE_INT_TYPE:
            case COLUMN_VALUE_2_BYTE_INT_TYPE:
            case COLUMN_VALUE_3_BYTE_INT_TYPE:
            case COLUMN_VALUE_4_BYTE_INT_TYPE:
                break;
            case COLUMN_VALUE_6_BYTE_INT_TYPE:
                type = 6;
                break;
            case COLUMN_VALUE_8_BYTE_INT_TYPE:
            case COLUMN_VALUE_FLOAT_TYPE:
                type = 8;
                break;
            default: {
                if (type >= 13 && type % 2 == 1) {
                    //String
                    type = COLUMN_VALUE_STRING_SIZE(type);
                } else if (type >= 12 && type % 2 == 0) {
                    //blob
                    type = COLUMN_VALUE_BLOB_SIZE(type);
                }
                is->seekg(type, std::ios_base::cur);
            }
        }
    }
}

uint64_t countWithWhereClause(std::ifstream* is, int pageNum, int columnNo, void* value, int pageSize){

    int64_t fileOffset = (int64_t)(pageSize)*(pageNum-1);
    if(pageNum==1){
        //Skip DB Header
        fileOffset += HEADER_SIZE;
    }
    is->seekg(fileOffset+BTREE_TYPE_OFFSET_1);
    char c;
    is->read(&c, 1);
    uint64_t ret = 0;
    switch(c){
        case LEAF_TABLE_B_TREE_TYPE: {

            is->seekg(fileOffset + NUMBER_OF_CELLS_OFFSET_2);
            uint16_t numCell = bigEndian(is, 2);

            is->seekg(fileOffset + CELL_CONTENT_AREA_START_2);
            uint16_t fileContentAreaStart = bigEndian(is, 2);

            is->seekg( fileContentAreaStart);
            for (int cell=0; cell<numCell; cell++){

                uint64_t cellPayloadSize = bigEndianVarInt(is);
                rowId_t key = bigEndianVarInt(is);

                uint64_t payloadStartOffset = is->tellg();

                uint64_t payloadHeaderSize = bigEndianVarInt(is);

                std::vector<uint64_t> types;

                for (int i = 0; i < columnNo ; i++) {
                    types.push_back(bigEndianVarInt(is));
                }

                is->seekg(payloadStartOffset + payloadHeaderSize);

                skipColumnValues(is, types);
                uint64_t type = types.back();

                assert(type>=13 && type%2==1);
                type = COLUMN_VALUE_STRING_SIZE(type);
                char curValue[type+1];
                curValue[type] = '\0';
                is->read(curValue, 5);
                if(strcmp(curValue, (char*)value)==0)
                    ret+=1;

                is->seekg(payloadStartOffset+cellPayloadSize);
            }
            break;
        }
        case INTERIOR_TABLE_BTREE_TYPE:
        case LEAF_INDEX_B_TREE_TYPE:
        case INTERIOR_INDEX_BTREE_TYPE:
            printf("Unsupported Format\n");
            break;
        default:
            printf("Error Page Type %d\n", c);
    }
    return ret;
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
