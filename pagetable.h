#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <bitset>

#define LOGICAL_ADDRESSS_SPACE 32 // Maximum logical address space

// forward declarations
typedef struct _LEVEL LEVEL;
typedef struct _MAP MAP;

// structure containing all components of your pagetable
typedef struct{
    LEVEL *rootNodePtr; // Points to level 0 root node

    int levelCount; // Specifies the amount of levels in the page tree
    uint32_t *shiftArray; // number of bits to shift at i level
    uint32_t *entryCount; // number of possible pages at i level
    uint32_t *bitmaskArray;  // bit mask at level i

}PAGETABLE;

// describes specific level of the page table
typedef struct _LEVEL{
    PAGETABLE *pageTablePtr; // points to page table
    
    int depth; // what depth the current level is on
    LEVEL **nextLevelPtr; // Points to next level for branches
    MAP *mapPtr; // Points to map for leaves
}LEVEL;

// MAP contains an array which maps from a logical page to physical frame
typedef struct _MAP{
    int *valid; // specifies whether a certain map value is valid 
    int *map; // holds frame number at specified map value
}MAP;

/*
* initializePageTable
* given a page table, tree levels, and level count, populate a page table with the 
* necessary information to read a trace file. sets level count, shift array, entry count
* and bitmask array
*/
void initializePageTable(PAGETABLE *pageTable, int treeLevels[], int levelCount);

/*
* PageInsert
* Will insert a page into the page table given a page table, address, and frame
*/
void pageInsert(PAGETABLE *pageTable, uint32_t address, int frame);

/*
* pageLookup
* Looks up a page in a page table given a logical address. returns MAP of that page.
* If the page does not exist returns nullptr.
*/
MAP * pageLookup(PAGETABLE * pageTable, uint32_t logicalAddress);

/*
* binToHex
* converts binary bitset object to hex value
*/
uint32_t binToHex(std::bitset<32> binVal);

/*
* populatePageArray
* given a level, empty page array, and address, returns a page array with all
* the pages of a specific address
*/
uint32_t *populatePageArray(LEVEL *level, uint32_t *pageArray, uint32_t address);

/*
* getOffset
* given a level and address returns the offset of the address
*/
uint32_t getOffset(LEVEL *level ,uint32_t address);

/*
* getlogical2physical
* given a level, logical address, and the tree levels, returns the physical address 
* of the logical address
*/
uint32_t getlogical2physical(LEVEL *level, uint32_t logicalAddress, int treeLevels[]);

/*
* getFrame
* given a level and logical address, returns the frame of the logical address
*/
int getFrame(LEVEL * level, uint32_t logicalAddress);

/*
* getPageSize
* given a level and tree levels, returns the page size
*/
uint32_t getPageSize(LEVEL *level, int treeLevels[]);

/*
* getBytes
* returns the total number of bytes used by the data structure
*/
uint32_t getBytes(PAGETABLE *pagetable);

#endif // PAGETABLE_H
