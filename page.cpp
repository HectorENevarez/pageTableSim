#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <math.h>

#include "pagetable.h"

uint32_t getBytes(LEVEL *level); // overloads function and takes in level, returns byte count for a level
bool leafNodeCheck(LEVEL *level); // checks if a level is a leaf node
MAP * pageLookup(LEVEL * level, uint32_t logicalAddress); // overloads function to check a specific level
void pageInsert(LEVEL * level, uint32_t address, int frame); // overloads function to insert at a specific level
uint32_t logicalToPage(uint32_t logicalAddress, uint32_t mask, uint32_t shift); // converts logical address  to page

// insert page called from main
void pageInsert(PAGETABLE *pageTable, uint32_t address, int frame){
    if (pageTable->rootNodePtr == nullptr) { // sets root note if not set
        pageTable->rootNodePtr = new LEVEL;
        pageTable->rootNodePtr->depth = 0; // root node depth
        pageTable->rootNodePtr->pageTablePtr = pageTable;
    }
    pageInsert(pageTable->rootNodePtr, address, frame);
};

// overloaded function
void pageInsert(LEVEL *level, uint32_t address, int frame){
    uint32_t page; 
    page = logicalToPage(address, level->pageTablePtr->bitmaskArray[level->depth], level->pageTablePtr->shiftArray[level->depth]); 

    if (leafNodeCheck(level)){ // if leaf node
        if (level->mapPtr == nullptr){ // if map doesn't exist sets map pointer
            level->mapPtr = new MAP;
        
            level->mapPtr->map = new int[level->pageTablePtr->entryCount[level->depth]]{0}; // initialize map array
            level->mapPtr->valid = new int[level->pageTablePtr->entryCount[level->depth]]{0}; // initialize valid array

            level->mapPtr->valid[page] = 1; // Changes valid bit from 1 to 0
            level->mapPtr->map[page] = frame;
        }
        else if (level->mapPtr->valid[page] != 1){ // if map exists
            level->mapPtr->valid[page] = 1; // Changes valid bit from 1 to 0
            level->mapPtr->map[page] = frame;
        }
    }

    else if (level->nextLevelPtr == nullptr){ // if next level pointer is not set
        level->nextLevelPtr = new LEVEL*[level->pageTablePtr->entryCount[level->depth]]{nullptr}; // initialize next level pointer

        level->nextLevelPtr[page] = new LEVEL; // intialize level at page
        level->nextLevelPtr[page]->pageTablePtr = level->pageTablePtr;
        level->nextLevelPtr[page]->depth = level->depth + 1; // updates level by one
        pageInsert(level->nextLevelPtr[page], address, frame); // recursively insert page until reaching leaf node
    }

    else if (level->nextLevelPtr[page] == nullptr){ // if next level pointer exists but not level at page
        level->nextLevelPtr[page] = new LEVEL; // declare new level
        level->nextLevelPtr[page]->pageTablePtr = level->pageTablePtr;
        level->nextLevelPtr[page]->depth = level->depth + 1; // updates level by one
        pageInsert(level->nextLevelPtr[page], address, frame); // recursively insert page until reaching leaf node
    }
    else{
        pageInsert(level->nextLevelPtr[page], address, frame); // recursively insert page until reaching leaf node
    }
};

// looks up page in a page table
MAP * pageLookup(PAGETABLE * pageTable, uint32_t logicalAddress){
    if (pageTable->rootNodePtr == nullptr) return nullptr; // if no root node pointer

    return pageLookup(pageTable->rootNodePtr, logicalAddress);
}

// overloaded function
MAP * pageLookup(LEVEL * level, uint32_t logicalAddress){
    uint32_t page;
    page = logicalToPage(logicalAddress, level->pageTablePtr->bitmaskArray[level->depth], level->pageTablePtr->shiftArray[level->depth]);
    
    if (leafNodeCheck(level)){ // if leaf node 
        // returns nullptr if mapPtr doesn't exist else return the map
        if (level->mapPtr->valid[page] != 1)
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

// returns page of logical address
uint32_t logicalToPage(uint32_t logicalAddress, uint32_t mask, uint32_t shift){
    uint32_t page;
    
    // page = (address & mask) >> shift
    page = logicalAddress & mask;
    page = page >> shift;

    return page;
};

// checks if level is leaf node
bool leafNodeCheck(LEVEL *level){
    return level->depth == (level->pageTablePtr->levelCount - 1); // subtract 1 because depth is 0 indexed level count is not 
};

// returns array of all page level of an address
uint32_t * populatePageArray(LEVEL *level, uint32_t *pageArray, uint32_t address){
    uint32_t page;
    page = logicalToPage(address, level->pageTablePtr->bitmaskArray[level->depth], level->pageTablePtr->shiftArray[level->depth]);
    pageArray[level->depth] = page;
    
    if (leafNodeCheck(level)) return pageArray;
    
    return populatePageArray(level->nextLevelPtr[page], pageArray, address); // recursively iterates through tree
};

// returns offset of an address
uint32_t getOffset(LEVEL *level ,uint32_t address){
    int levelCount = level->pageTablePtr->levelCount;
    uint32_t pageBits = 0;
    uint32_t offset;

    int idx = 0;
    for (; idx < levelCount; idx++){
        pageBits += level->pageTablePtr->bitmaskArray[idx]; // grabs all page bits
    }
    offset = address & ~pageBits; // flip page bits to get offset bits
    return offset;
};

// gets frame of a logical address
int getFrame(LEVEL * level, uint32_t logicalAddress){
    uint32_t page;
    page = logicalToPage(logicalAddress, level->pageTablePtr->bitmaskArray[level->depth], level->pageTablePtr->shiftArray[level->depth]);
    
    if (leafNodeCheck(level)){ // if leaf node 
        return level->mapPtr->map[page]; //returns frame
    }
    else{
        return getFrame(level->nextLevelPtr[page], logicalAddress); // recursively iterates through tree
    }
};

// returns page size of a page table
uint32_t getPageSize(LEVEL *level, int treeLevels[]){
    int levelCount = level->pageTablePtr->levelCount;
    uint32_t pageSize = 0;

    int idx;
    for (idx = 0; idx < levelCount; idx++){
        pageSize += treeLevels[idx]; // grabs all tree levels page bits
    }

    pageSize = LOGICAL_ADDRESSS_SPACE - pageSize; // address space is 32 bits
    pageSize = pow(2, pageSize); // 2 to the page size

    return pageSize;
};

// returns physical address given a logical address
uint32_t getlogical2physical(LEVEL *level, uint32_t logicalAddress, int treeLevels[]){
    int pageSize = getPageSize(level, treeLevels);
    int frame = getFrame(level, logicalAddress);
    uint32_t offset = getOffset(level, logicalAddress);
    uint32_t pAddr;

    // physical address = (frame * page size) + offset
    pAddr = frame * pageSize;
    pAddr += offset;
    return pAddr;
};

// converts binary bitset to hex
uint32_t binToHex(std::bitset<32> binVal){
    unsigned long binLong = binVal.to_ulong();
    std::stringstream ss;
    uint32_t hexVal;

    ss << std::hex << binLong;
    ss >> std::hex >> hexVal;
    
    return hexVal;
};

// returns total bytes of page tree
uint32_t getBytes(PAGETABLE *pagetable){
    uint32_t totalBytes = 0;

    // grab bytes used by pageTable struct
    totalBytes += sizeof(pagetable->bitmaskArray);
    totalBytes += sizeof(pagetable->entryCount);
    totalBytes += sizeof(pagetable->levelCount);
    totalBytes += sizeof(pagetable->shiftArray);
    totalBytes += sizeof(pagetable->rootNodePtr);

    totalBytes += getBytes(pagetable->rootNodePtr);

    return totalBytes;
};

// overloaded function
// returns bytes of every level
uint32_t getBytes(LEVEL *level){
    int idx = 0;
    uint32_t totalBytes = 0;
    uint32_t entryCount = level->pageTablePtr->entryCount[level->depth];
    
    totalBytes += sizeof(level->pageTablePtr);
    totalBytes += sizeof(level->depth);
    for (;idx < entryCount; idx++){ // check every next level pointer
        if (leafNodeCheck(level)){
            totalBytes += sizeof(level->mapPtr->valid[idx]);
            totalBytes += sizeof(level->mapPtr->map[idx]);
        }
        else if (level->nextLevelPtr[idx] != nullptr){
            totalBytes += getBytes(level->nextLevelPtr[idx]); // recursively iterates through every level
        }
        else{
            totalBytes += sizeof(level->nextLevelPtr[idx]);
        }
    }
    return totalBytes;

};