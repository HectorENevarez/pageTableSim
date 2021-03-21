#include <string>
#include <sstream>
#include <bitset>
#include <iomanip>

#include <iostream>

#include "base_helpers.h"

unsigned int binToHex(std::bitset<32> binVal){
    unsigned long binLong = binVal.to_ulong();
    std::stringstream ss;
    unsigned int hexVal;

    ss << std::hex << std::setw(8) << std::setfill('0') << binLong;
    ss >> std::hex >> hexVal;

    //printf("%X\n", hexVal);
    //std::cout << hexVal << std::endl;
    
    return hexVal;
}

unsigned int hexSTUI (std::string address){
    unsigned int hexVal;
    std::stringstream ss;
    ss << std::hex << address;
    ss >> hexVal;

    //std::cout << std::hex << std::setw(8) << std::setfill('0') << hexVal << std::endl;

    return hexVal;

}