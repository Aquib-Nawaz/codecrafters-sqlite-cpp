//
// Created by Aquib Nawaz on 18/02/24.
//

#include "page.h"
#include "utility.h"

Page::Page(std::ifstream *is, uint32_t pageNum, uint32_t pageSize, bool getCells){
    uint64_t pageOffset;
    int64_t fileOffset = pageOffset = (int64_t)(pageSize)*(pageNum-1);
    if(pageNum==1){
        //Skip DB Header
        fileOffset += HEADER_SIZE;
    }

    is->seekg(fileOffset+BTREE_TYPE_OFFSET_1);
    pageType = bigEndian(is,1);

    is->seekg(fileOffset + NUMBER_OF_CELLS_OFFSET_2);
    numCell = bigEndian(is, 2);

    if(getCells || pageType == INTERIOR_INDEX_BTREE_TYPE || pageType == INTERIOR_TABLE_BTREE_TYPE){
        cellsOffset.reserve(numCell);

        uint64_t headerEnd = fileOffset + 8;

        if(pageType == INTERIOR_INDEX_BTREE_TYPE || pageType == INTERIOR_TABLE_BTREE_TYPE){
            is->seekg(fileOffset+LAST_POINTER_PAGE_NUMBER_OFFSET_4);
            lastPageNum = bigEndian(is, 4);
            headerEnd += 4;
        }

        is->seekg(headerEnd);
        for(int i=0; i<numCell; i++){
            cellsOffset.push_back(pageOffset+ bigEndian(is, 2));
        }
    }
}