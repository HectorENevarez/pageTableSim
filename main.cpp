#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "byutr.h"
#include "pagetable.h"
#include "output_mode_helpers.h"
//#include "base_helpers.h"

int main(int argc, char **argv) {
    int Option;
    int Nflag ; // Addresses to process
    int maxCounter = 0; // makes sure no more than 32 bits are assigned
    int traceCounter = 0; // counts addresses traced to compare with Nflag
    std::string outMethod; // output function
    
    FILE *tFile; //tFile is used to read the trace files
    std::string address; // address traced

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
    pageTable->levelCount = sizeof(treeLevels)/sizeof(treeLevels[0]);

    setBitMaskAry(pageTable, treeLevels);
    setEntryCount(pageTable, treeLevels);
    setShiftArray(pageTable, treeLevels);
    
    /*
    TODO
    this portion should directly call the page functions necessary to do the 
    processing
    */

    // reads trace file
    p2AddrTr trace_item;
    bool done = false;
    while(!done){
        int bytesread = NextAddress(tFile, &trace_item);
        done = bytesread == 0;

        if (!done && (traceCounter < Nflag))
        address = trace_item.addr;
        std::cout << address;
    
        traceCounter++;
    }

    


    fclose(tFile); // file close
};
