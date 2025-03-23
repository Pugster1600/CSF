#include <iostream>
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>
#include <map>
#include "cache.h"

bool isPowerOf2(uint32_t val){
  return (((val) & (val - 1)) == 0);
}

bool isValidCacheWritePolicies(std::string writeMissPolicy, std::string writeHitPolicy){
  if (writeMissPolicy == "no-write-allocate") {
    if (writeHitPolicy == "write-back") {
      return false;
    }
  }
  return true;
}

void checkValidInputs(){
  
}
int main( int argc, char **argv ) {
  // TODO: implement
  if (argc != 7) {
    std::cerr << "number of arguments not equal to 7\n" << std::endl;
    return 1;
  }

  uint32_t totalSets = static_cast<uint32_t>(std::stoul(argv[1]));
  uint32_t kAssociativity = static_cast<uint32_t>(std::stoul(argv[2]));
  uint32_t sizePerBlock = static_cast<uint32_t>(std::stoul(argv[3]));
  std::string cacheWriteMissPolicy = argv[4];
  std::string cacheWriteHitPolicy = argv[5];
  std::string cacheEvictionPolicy = argv[6];

  if (!isPowerOf2(totalSets)) {
    std::cerr << "number of sets is not power of 2!\n" << std::endl;
    return 1;
  }

  if (!isPowerOf2(kAssociativity)) {
    std::cerr << "number of lines per set is not power of 2!\n" << std::endl;
    return 1;
  }

  if (!isPowerOf2(sizePerBlock)) {
    std::cerr << "Data size of each line is not power of 2!\n" << std::endl;
    return 1;
  } else if (sizePerBlock < 4){
    std::cerr << "Data size per block is less than 4!\n" << std::endl;
    return 1;
  }

  if (!isValidCacheWritePolicies(cacheWriteMissPolicy, cacheWriteHitPolicy)){
    std::cerr << "Cannot use both no-write-allocate and write-through together" << std::endl;
    return 1;
  }

  Cache cache(totalSets, kAssociativity, sizePerBlock, cacheWriteMissPolicy, cacheWriteHitPolicy, cacheEvictionPolicy);

  cache.mainLoop();

  return 0;
}

//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign03/solution.zip .