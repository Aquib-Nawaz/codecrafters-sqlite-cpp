//
// Created by Aquib Nawaz on 18/02/24.
//

#ifndef GIT_STARTER_CPP_PAGE_H
#define GIT_STARTER_CPP_PAGE_H

#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>

#define HEADER_SIZE 100 //For Root Page only
#define BTREE_TYPE_OFFSET_1 0
#define NUMBER_OF_CELLS_OFFSET_2 3

#define INTERIOR_INDEX_BTREE_TYPE 2
#define INTERIOR_TABLE_BTREE_TYPE 5
#define LEAF_INDEX_B_TREE_TYPE 10
#define LEAF_TABLE_B_TREE_TYPE 13
#define LAST_POINTER_PAGE_NUMBER_OFFSET_4 8


class Page {
    public:
    uint64_t numCell;
    std::vector<uint64_t>cellsOffset;
    int pageType;
    uint32_t lastPageNum;
    Page(std::ifstream *is, uint32_t pageNum, uint32_t, bool getCells=1);

};
#endif //GIT_STARTER_CPP_PAGE_H
