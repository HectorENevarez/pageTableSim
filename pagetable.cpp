#include <math.h> // pow

#include "pagetable.h"

void setBitMaskAry(PAGETABLE *pageTable, int treeLevels[]); // sets the bit mask array of a specified page table
void setEntryCount(PAGETABLE *pageTable, int treeLevels[]); // sets the entry count array of a specified page table
void setShiftArray(PAGETABLE *pageTable, int treeLevels[]); // sets the shift array of a specified page table

// initialize page table
void initializePageTable(PAGETABLE *pageTable, int treeLevels[], int levelCount){
    pageTable->levelCount = levelCount; // sets level count

    // setting all fields of the structure
    setBitMaskAry(pageTable, treeLevels);
    setEntryCount(pageTable, treeLevels);
    setShiftArray(pageTable, treeLevels);
};

void setBitMaskAry(PAGETABLE *pageTable, int treeLevels[]){
    pageTable->bitmaskArray = new unsigned int[pageTable->levelCount]; //dynamically create array
    
    int location = LOGICAL_ADDRESSS_SPACE - 1; // bit set location is 0 indexed
    std::bitset<LOGICAL_ADDRESSS_SPACE> bit_init (0); // set all bits to zero

    for(int i = 0; i < pageTable->levelCount; i++){ // First loop iterates through every level
        
        for (int j = location; j > location - treeLevels[i]; j--){ // iterates through every bit
            bit_init.flip(j);
        }
        pageTable->bitmaskArray[i] = binToHex(bit_init);

        location -= treeLevels[i];
        bit_init.reset(); // resets bits
    }
};

void setEntryCount(PAGETABLE *pageTable, int treeLevels[]){
    pageTable->entryCount = new unsigned int[pageTable->levelCount]; //dynamically create array
    for (int i = 0; i < pageTable->levelCount; i++){
        pageTable->entryCount[i] = pow(2, treeLevels[i]);
    }
};

void setShiftArray(PAGETABLE *pageTable, int treeLevels[]){
    pageTable->shiftArray = new unsigned int[pageTable->levelCount]; //dynamically create array
    unsigned int shiftArrVal = LOGICAL_ADDRESSS_SPACE;
    for (int i = 0; i < pageTable->levelCount; i++){
        shiftArrVal -= treeLevels[i];
        pageTable->shiftArray[i] = shiftArrVal;
    }
};
