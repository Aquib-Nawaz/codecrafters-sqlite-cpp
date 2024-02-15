//
// Created by Aquib Nawaz on 15/02/24.
//

#ifndef GIT_STARTER_CPP_BTREE_H
#define GIT_STARTER_CPP_BTREE_H

#include <cstdint>
#include <fstream>
#include <iostream>

#define INDEX_TABLE_KEY_MAX_LENGTH 2147483647
//In header
#define RESERVED_REGION_SIZE_OFFSET 20

//The Actuala location of the keys in page is arbitary
//But Logically the are sorted ()
//Interior B Tree P K P K P
// P K are combined to form cell last P is alone like me
// WITHOUT_ROW_ID table uses index b-trees

/********* Page Structure **************/
//Integer are Big Endian

#define HEADER_SIZE 100 //For Root Page only
#define LEAF_BTREE_HEADER_SIZE 8 //OR 12
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

uint64_t countWithWhereClause(std::ifstream*, int pageNum, int columnNo, void* value, int pageSize);
uint64_t bigEndianVarInt(std::ifstream* , int maxLength=9);
uint64_t bigEndian(std::ifstream* , int length);


//The Reserved region



#endif //GIT_STARTER_CPP_BTREE_H
