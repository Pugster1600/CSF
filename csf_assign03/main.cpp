#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include <cmath>
#include <cassert>

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


uint32_t calculateTotalSetBits(uint32_t totalSets){
  return static_cast<uint32_t>(log2(totalSets)); //if 256 sets, 8 bits needed to find which set
}

uint32_t calculateTotalOffsetBits(uint32_t sizePerBlock){
  uint32_t sectionsPerBlock = sizePerBlock / 4;
  return static_cast<uint32_t>(log2(sectionsPerBlock));  //if each line/block has 16 bytes, then we have 4 offsets
}

uint32_t calculateTotalCacheSize(uint32_t totalSets, uint32_t kAssociativity, uint32_t sizePerBlock){
  return totalSets * sizePerBlock * kAssociativity; //in bytes, 256 sets * 16 bytes/line, 4 lines/set = units of bytes
}

uint32_t calculateTotalTagBits(uint32_t totalSets, uint32_t sizePerBlock){
  uint32_t fullAddressBits = 32;
  uint32_t setBits = calculateTotalSetBits(totalSets); //bits to figure out which set we are in (then we match tag)
  uint32_t offsetBits = calculateTotalOffsetBits(sizePerBlock); //bits to figure out which section of the line/block we are in
  return fullAddressBits - setBits - offsetBits;
}

uint32_t hexToUL(std::string hex){
  return std::stoul(hex, nullptr, 16);
}

uint32_t getTagValue(uint32_t totalTagBits, uint32_t hex){
  return hex >> (32 - totalTagBits); //tag, set, offset
}

uint32_t getOffsetValue(uint32_t totalOffsetBits, uint32_t hex){
  uint32_t mask = (1U << totalOffsetBits) - 1; //0b100 -> 0b11
  return hex & mask;
}

uint32_t getSetValue(uint32_t totalSetBits, uint32_t totalOffsetBits, uint32_t hex){
  uint32_t hexWithoutOffset = hex >> totalOffsetBits;
  uint32_t mask = (1U << totalSetBits) - 1;
  return hexWithoutOffset & mask;
}

void testHelperFunctions(uint32_t totalSets, uint32_t kAssociativity, uint32_t sizePerBlock){
  std::string testVal = "0xdeadbeef";
  uint32_t val = hexToUL(testVal);
  assert(val == 0xdeadbeef);
  std::cout << val << std::endl;

  //256 sets = 2^8, associativity = 4, sizePerBlock = 16
  //22 tag bits, 8 set bits and 2 offset bits 

  uint32_t totalOffsetBits = calculateTotalOffsetBits(sizePerBlock);
  uint32_t totalSetBits = calculateTotalSetBits(totalSets);
  uint32_t totalTagBits = calculateTotalTagBits(totalSets, sizePerBlock);

  assert(totalOffsetBits == 2);
  assert(totalSetBits == 8);
  assert(totalTagBits == 22);

  uint32_t offsetValue = getOffsetValue(totalOffsetBits, val);
  uint32_t setValue = getSetValue(totalSetBits, totalOffsetBits, val);
  uint32_t tagValue = getTagValue(totalTagBits, val);
  assert(offsetValue == 0b11); //0xdeadbee f -> 0xf = 0b11 11
  assert(setValue == 0b10111011); //0xdeadb eef -> 0xeef = 11 10 1110 11 11 -> 10 1110 11
  assert(tagValue == 0b1101111010101101101111); //0xdeadb = 0b11011110101011011011, then add 11
}

void readFile(){
  std::string line;

  // Read lines from standard input (stdin)
  while (std::getline(std::cin, line)) {
    std::istringstream stream(line);
    std::string word;

    for (int i = 0; i < 3; i++){
      stream >> word;
      std::cout << word << " ";
    }
    std::cout << std::endl;
  }
}

class directMappedCache{
public:
  directMappedCache(uint32_t totalSets, uint32_t kAssociativity, uint32_t sizePerBlock){

  }
private:

};


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

  testHelperFunctions(totalSets, kAssociativity, sizePerBlock);

  return 0;
}
