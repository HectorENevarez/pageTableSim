#include <iostream>
#include <unistd.h>

#include "byutr.h"
#include "pagetable.h"
#include "output_mode_helpers.h"

int main(int argc, char **argv) {
    int Option;
    int Nflag ; // Addresses to process
    int maxCounter = 0; // makes sure no more than 32 bits are assigned
    int levelCount; // amount of levels in the tree
    uint32_t traceCounter = 0; // counts addresses traced to compare with Nflag
    std::string outMethod; // output function
    
    FILE *tFile; //tFile is used to read the trace files
    uint32_t frame = 0; // counts frames

    // argument parser
    while ( (Option = getopt(argc, argv, "n:o:")) != -1){
        switch (Option){
            case 'n':
                Nflag = atoi(optarg);
                break;
            case 'o':
                outMethod = optarg;
                break;
            default:
                exit(0);
        }
    }

    // makes sure mandatory arguments are specified
    if (optind == argc || optind + 1 == argc){
        std::cout << "You must include a path and the size of atleast one tree level" << std::endl;
        exit(0);
    }

    // opens file from command line argument
    tFile = fopen(argv[optind], "r");
    optind++; 
    
    // Checks if file can be opened
    if (tFile == NULL){ 
        std::cout << "Error opening file" << std::endl; 
        exit(0);
    }
    
    // Assigns levels to array
    int treeLevels[argc - optind];
    for (int idx = 0; optind < argc; optind++, idx++){
        treeLevels[idx] = atoi(argv[optind]);

        // Makes sure user doesn't input more than 32 bits
        maxCounter += treeLevels[idx];
        if (maxCounter > LOGICAL_ADDRESSS_SPACE){
            std::cout << "You may only assign 32 bits" << std::endl;
            exit(0);
        }
    }

    // initialize page table
    PAGETABLE *pageTable = new PAGETABLE;
    levelCount = sizeof(treeLevels)/sizeof(treeLevels[0]);

    initializePageTable(pageTable, treeLevels, levelCount);

    // local vars
    uint32_t pageArray[pageTable->levelCount]; // stores pages of a specific address
    uint32_t offset; // stores offset of an address
    uint32_t pageFrame; 
    uint32_t pAddr; // physical address
    uint32_t hit = 0;
    uint32_t pageSize;
    uint32_t bytesUsed;

    // reads trace file
    p2AddrTr trace_item;
    bool done = false;
    while(!done && (traceCounter < Nflag)){
        int bytesread = NextAddress(tFile, &trace_item);
        done = bytesread == 0;

        if (!done && (traceCounter < Nflag)){
            if (pageLookup(pageTable, trace_item.addr) == nullptr){ // check if page exists
                pageInsert(pageTable, trace_item.addr, frame); // creates new page
                frame++;
            }

            if (outMethod == "offset"){ // prints out offset of each address
                offset = getOffset(pageTable->rootNodePtr, trace_item.addr); // returns offset
                report_logical2offset(trace_item.addr, offset);
            }
            else if (outMethod == "page2frame"){ // prints frame of every page
                populatePageArray(pageTable->rootNodePtr, pageArray, trace_item.addr); // returns page array
                pageFrame = getFrame(pageTable->rootNodePtr, trace_item.addr); // returns page frame
                
                report_pagemap(trace_item.addr, pageTable->levelCount, pageArray, pageFrame);
            }
            else if (outMethod == "logical2physical"){ // prints physical address of every addres
                pAddr = getlogical2physical(pageTable->rootNodePtr, trace_item.addr, treeLevels); // returns physical address from logical address
                report_logical2physical(trace_item.addr, pAddr);
            }
        }
        traceCounter++; // counts total addresses
    }
    
    if (outMethod == "summary"){ // prints summary
        hit = traceCounter - frame;
        pageSize = getPageSize(pageTable->rootNodePtr, treeLevels); // returns page size
        bytesUsed = getBytes(pageTable); // returns bytes used
        
        report_summary(pageSize,hit,traceCounter,frame,bytesUsed);
    }

    if (outMethod == "bitmasks"){ // prints bitmasks
        report_bitmasks(pageTable->levelCount, pageTable->bitmaskArray);
    }
    
    fclose(tFile); // file close
};


