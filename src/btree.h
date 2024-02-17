//
// Created by Aquib Nawaz on 15/02/24.
//

#ifndef GIT_STARTER_CPP_BTREE_H
#define GIT_STARTER_CPP_BTREE_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <cctype>
#include "utility.h"

template<class T>
const T& max(const T& a, const T& b)
{
    return (a < b) ? b : a;
}



#define INDEX_TABLE_KEY_MAX_LENGTH 2147483647
//In header
#define RESERVED_REGION_SIZE_OFFSET 20

//The Actual location of the keys in page is arbitrary
//But Logically the are sorted ()
//Interior B Tree P K P K P
// P K are combined to form cell last P is alone like me
// WITHOUT_ROW_ID table uses index b-trees

/********* Page Structure **************/
//Integer are Big Endian

#define HEADER_SIZE 100 //For Root Page only
#define LEAF_BTREE_HEADER_SIZE 8
#define INTERIOR_BTREE_HEADER_SIZE 12

//BTREE HEADER
#define BTREE_TYPE_OFFSET_1 0

#define INTERIOR_INDEX_BTREE_TYPE 2
#define INTERIOR_TABLE_BTREE_TYPE 5
#define LEAF_INDEX_B_TREE_TYPE 10
#define LEAF_TABLE_B_TREE_TYPE 13

#define FIRST_FREE_BLOCK_START_OFFSET_2 1
#define NUMBER_OF_CELLS_OFFSET_2 3
#define CELL_CONTENT_AREA_START_2 5
#define NUMBER_OF_FRAGMENTED_BYTE_OFFSET_1 7
//For interior Btree pages only
#define LAST_POINTER_PAGE_NUMBER_OFFSET_4 8

//Cell Pointer Array
typedef uint16_t cellOffset_t;
//2 byte integer * Number of cell
//Arranged in Key Order

//Unallocated Space

//The Cell Content Area

//Payload-Header-KeyValue
//varint-size , varint rowid, varint recordheader-size, varint-types, varint values

//Key is unsignedint64

typedef uint64_t rowId_t;


//Array of record
#define COLUMN_VALUE_NULL_TYPE 0
#define COLUMN_VALUE_1_BYTE_INT_TYPE 1
#define COLUMN_VALUE_2_BYTE_INT_TYPE 2
#define COLUMN_VALUE_3_BYTE_INT_TYPE 3
#define COLUMN_VALUE_4_BYTE_INT_TYPE 4
#define COLUMN_VALUE_6_BYTE_INT_TYPE 5
#define COLUMN_VALUE_8_BYTE_INT_TYPE 6
#define COLUMN_VALUE_FLOAT_TYPE 7

#define COLUMN_VALUE_BLOB_SIZE(size) ((size-12)/2)
#define COLUMN_VALUE_STRING_SIZE(size) ((size-13)/2)

#define SQLITE_SCHEMA_TYPE_COLUMN 1
#define SQLITE_SCHEMA_NAME_COLUMN 2
#define SQLITE_SCHEMA_PAGE_NUM_COLUMN 4
#define SQLITE_SCHEMA_TEXT_COLUMN 5

//The Reserved region

void skipColumnValues(std::ifstream *is, const std::vector<uint64_t> &types, int);
uint64_t countRows(std::ifstream *, int pageNum, int pageSize);

template<typename T>
T getColumn(std::ifstream *is, uint64_t type);

template<typename T, typename U>
void populateReturnColumnsList(std:: ifstream*, const U& ,
                               std::vector<T>*, uint64_t , const std::vector<uint64_t >&, uint64_t );

template<typename T, typename U>
uint64_t countWithWhereClause(std::ifstream* is, int pageNum, int columnNo, void* value, int pageSize,
                              U retColumnNum, std::vector<T>*returnList){

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
    is->seekg(fileOffset + CELL_CONTENT_AREA_START_2);
    uint16_t fileContentAreaStart = bigEndian(is, 2);

    switch(c){
        case LEAF_TABLE_B_TREE_TYPE: {
            is->seekg( fileContentAreaStart+(int64_t)(pageSize)*(pageNum-1));

            for (int cell=0; cell<numCell; cell++){

                uint64_t cellPayloadSize = bigEndianVarInt(is);
                rowId_t key = bigEndianVarInt(is);

                uint64_t payloadStartOffset = is->tellg();

                uint64_t payloadHeaderSize = bigEndianVarInt(is);

                std::vector<uint64_t> types;

                for (int i = 0; i < max(columnNo, getMax(retColumnNum)) ; i++) {
                    types.push_back(bigEndianVarInt(is));
                }
                bool currentRecordMatches = true;
                uint64_t type;
                if(columnNo !=-1){
                    is->seekg(payloadStartOffset + payloadHeaderSize);

                    skipColumnValues(is, types, columnNo-1);
                    type = types[columnNo-1];

                    char* curValue = getColumn<char *>(is, type);
                    currentRecordMatches = strcmp(curValue, (char*)value)==0;
                    free(curValue);
                }
                if(currentRecordMatches){
                    ret+=1;
                    populateReturnColumnsList(is, retColumnNum, returnList, payloadStartOffset
                                                                            + payloadHeaderSize, types, key);
                }
                is->seekg(payloadStartOffset+cellPayloadSize);
            }
            break;
        }
        case INTERIOR_TABLE_BTREE_TYPE:{
            is->seekg(fileOffset+LAST_POINTER_PAGE_NUMBER_OFFSET_4);
            uint32_t lastPageNum = bigEndian(is, 4);
            is->seekg( fileContentAreaStart+(int64_t)(pageSize)*(pageNum-1));

            for (int cell=0; cell<numCell; cell++){
                uint64_t childPageNum = bigEndian(is,4);
                fileOffset = is->tellg();
                countWithWhereClause(is,childPageNum, columnNo,value, pageSize, retColumnNum,
                                     returnList);
                is->seekg(fileOffset);
                bigEndianVarInt(is);
            }
            countWithWhereClause(is,lastPageNum, columnNo,value, pageSize, retColumnNum,
                                 returnList);
            break;
        }
        case LEAF_INDEX_B_TREE_TYPE:
        case INTERIOR_INDEX_BTREE_TYPE:
            printf("Unsupported Format\n");
            break;
        default:
            printf("Error Page Type %d\n", c);
    }
    return ret;
}

#endif //GIT_STARTER_CPP_BTREE_H
