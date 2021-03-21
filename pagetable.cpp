#include <iostream>
#include <stdio.h>
#include <bitset>
#include <string>
#include <math.h> // pow
#include <iostream>
#include <fstream>

#include "pagetable.h"
#include "base_helpers.h"


// TODO delete create page table and implement from main
void createPageTable(PAGETABLE * pageTable, int treeLevels[], int levelCount){
    // load in addresses and start assigning

    int counter = 0; //tmep counter
    std::string addressStr;
    unsigned int address;
    unsigned int frame, hit, miss;

    std::ifstream addressFile;
    addressFile.open("addresses.txt");

    // test while loop
    frame = 0;
    hit = 0;
    miss = 0;
    while (getline(addressFile, addressStr)){
        std::cout << addressStr << std::endl;
        address = hexSTUI(addressStr);
        
        if (pageLookup(pageTable, address) == nullptr){
            pageInsert(pageTable, address, frame);
            frame++; // increments frame counter
            miss++;
        }
        else{
            hit++;
        }
        
        
        if (counter == 100000) break;
        counter++;
    }

    std::cout << "hit: " << hit << " miss " << miss;
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