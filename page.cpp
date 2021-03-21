#include <iostream>
#include <string>

#include "pagetable.h"

void pageInsert(PAGETABLE *pageTable, unsigned int address, int frame){
    if (pageTable->rootNodePtr == nullptr) {
        pageTable->rootNodePtr = new LEVEL;
        pageTable->rootNodePtr->depth = 0; // root node depth
        pageTable->rootNodePtr->pageTablePtr = pageTable;
    }
    pageInsert(pageTable->rootNodePtr, address, frame);
};

void pageInsert(LEVEL *level, unsigned int address, int frame){
    unsigned int page; 
    page = logicalToPage(address, level->pageTablePtr->bitmaskArray[level->depth], level->pageTablePtr->shiftArray[level->depth]); 

    if (leafNodeCheck(level)){ // if leaf node
        level->mapPtr = new MAP;
        
        level->mapPtr->map = new int[level->pageTablePtr->entryCount[level->depth]];
        level->mapPtr->valid = new int[level->pageTablePtr->entryCount[level->depth]];

        level->mapPtr->valid[page] = 1; // Changes valid bit from 1 to 0
        level->mapPtr->map[page] = frame;
    }

    else{
        level->nextLevelPtr = new LEVEL*[level->pageTablePtr->entryCount[level->depth]];

        level->nextLevelPtr[page] = new LEVEL;
        level->nextLevelPtr[page]->pageTablePtr = level->pageTablePtr;
        level->nextLevelPtr[page]->depth = level->depth + 1; // updates level by one
        pageInsert(level->nextLevelPtr[page], address, frame);
    }
};

MAP * pageLookup(PAGETABLE * pageTable, unsigned int logicalAddress){
    if (pageTable->rootNodePtr == nullptr) return nullptr;

    return pageLookup(pageTable->rootNodePtr, logicalAddress);
}

MAP * pageLookup(LEVEL * level, unsigned int logicalAddress){
    unsigned int page;
    page = logicalToPage(logicalAddress, level->pageTablePtr->bitmaskArray[level->depth], level->pageTablePtr->shiftArray[level->depth]);
    
    if (leafNodeCheck(level)){ // if leaf node 
        // returns nullptr if mapPtr doesn't exist else return the map
        if (level->mapPtr == nullptr)
            return nullptr; 
        else
            return level->mapPtr; 
    }
    else{
        // returns nullptr if nextLevelPtr doesn't exist else recursively calls pageLookup
        if (level->nextLevelPtr[page] == nullptr)
            return nullptr; 
        else 
            return pageLookup(level->nextLevelPtr[page], logicalAddress);
    }
}

unsigned int logicalToPage(unsigned int logicalAddress, unsigned int mask, unsigned int shift){
    unsigned int page;
    
    page = logicalAddress & mask;
    page = page >> shift;
    
    //printf("%X , %X, %u\n", page, mask, shift);

    return page;
};

bool leafNodeCheck(LEVEL *level){
    return level->depth == (level->pageTablePtr->levelCount - 1); // subtract 1 because depth is 0 indexed level count is not 
};