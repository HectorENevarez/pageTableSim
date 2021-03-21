#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdint.h>
#include <string>

#define LOGICAL_ADDRESSS_SPACE 32
#define ZERO_DEPTH 0

// forward declarations
typedef struct _LEVEL LEVEL;
typedef struct _MAP MAP;

// Contains information about the tree
typedef struct{
    LEVEL *rootNodePtr; // Points to level 0 root node

    int levelCount;
    unsigned int *shiftArray;
    unsigned int *entryCount;
    unsigned int *bitmaskArray; 
}PAGETABLE;

// describes level of the page table
typedef struct _LEVEL{
    PAGETABLE *pageTablePtr;
    
    int depth;
    LEVEL **nextLevelPtr; // Points to next level for branches
    MAP *mapPtr; // Points to map for leaves
}LEVEL;

// MAP contains an array which maps from a logical page to physical frame
typedef struct _MAP{
    int *valid;
    int *map;
}MAP;

void createPageTable(PAGETABLE * pageTable, int treeLevels[], int levelCount);
void setBitMaskAry(PAGETABLE *pageTable, int treeLevels[]);
void setEntryCount(PAGETABLE *pageTable, int treeLevels[]);
void setShiftArray(PAGETABLE *pageTable, int treeLevels[]);

void pageInsert(PAGETABLE *pageTable, unsigned int address, int frame);
void pageInsert(LEVEL * level, unsigned int address, int frame);

MAP * pageLookup(PAGETABLE * pageTable, unsigned int logicalAddress);
MAP * pageLookup(LEVEL * level, unsigned int logicalAddress);

unsigned int logicalToPage(unsigned int logicalAddress, unsigned int mask, unsigned int shift);

bool leafNodeCheck(LEVEL *level);

#endif // PAGETABLE_H
