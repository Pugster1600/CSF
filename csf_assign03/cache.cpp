#include "cache.h"

Cache::Cache(uint32_t _totalSets, uint32_t _kAssociativity, uint32_t _sizePerBlock, std::string _writeMissPolicy, std::string _writeHitPolicy, std::string _evictionPolicy)
:totalSets(_totalSets), kAssociativity(_kAssociativity), sizePerBlock(_sizePerBlock), writeMissPolicy(_writeMissPolicy), writeHitPolicy(_writeHitPolicy), evictionPolicy(_evictionPolicy){
  this -> totalSetBits = Cache::calculateTotalSetBits(this -> totalSets);
  this -> totalTagBits = Cache::calculateTotalTagBits(this -> totalSets, this -> sizePerBlock);
  this -> totalOffsetBits = Cache::calculateTotalOffsetBits(this -> sizePerBlock);

  for (std::map<uint32_t, std::vector<uint32_t>>::iterator it = cacheDataStructure.begin(); it != cacheDataStructure.end(); it++){
    (it->second).resize(this -> kAssociativity);
  }

  std::cout << "totalSets: " << totalSets << std::endl;
  std::cout << "kAssociativity: " << kAssociativity << std::endl;
  std::cout << "sizePerBlock: " << sizePerBlock << std::endl;
  std::cout << "writeMissPolicy: " << writeMissPolicy << std::endl;
  std::cout << "writeHitPolicy: " << writeHitPolicy << std::endl;
  std::cout << "evictionPolicy: " << evictionPolicy << std::endl;
  std::cout << "totalSetBits: " << totalSetBits << std::endl;
  std::cout << "totalTagBits: " << totalTagBits << std::endl;
  std::cout << "totalOffsetBits: " << totalOffsetBits << std::endl;
}

void Cache::loadData(uint32_t address){ //RAM -> cache
  uint32_t tag = getTagValue(this -> totalTagBits, address);
  uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
  uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address); //even if setBits = 0 ie directMapping, still works becuase array[0]
  if (this -> cacheDataStructure.size() > totalSets) { //eviction
    return; //
  }
}

void Cache::storeData(uint32_t address){ //cache -> RAM
  if (this -> cacheDataStructure.size() > totalSets) {
    return; //
  }
}

void Cache::mainLoop(){
  std::string line = Cache::readNextLine();
  std::istringstream stream(line);

  std::string operation;
  std::string hex;
  std::string extra;

  
  while (line != ""){
    stream >> operation;
    stream >> hex;
    stream >> extra;
    
    std:: cout << operation << " " << hex << " " << extra << std::endl;

    line = Cache::readNextLine();
    stream = std::istringstream(line);
  }
}

std::string Cache::readNextLine(){
  std::string line;

  if (std::getline(std::cin, line)) {
    return line;
  } else {
    return "";
  }
}

uint32_t Cache::calculateTotalSetBits(uint32_t totalSets){
  return static_cast<uint32_t>(log2(totalSets)); //if 256 sets, 8 bits needed to find which set
}

uint32_t Cache::calculateTotalOffsetBits(uint32_t sizePerBlock){
  uint32_t sectionsPerBlock = sizePerBlock / 4;
  return static_cast<uint32_t>(log2(sectionsPerBlock));  //if each line/block has 16 bytes, then we have 4 offsets
}

uint32_t Cache::calculateTotalTagBits(uint32_t totalSets, uint32_t sizePerBlock){
  uint32_t fullAddressBits = 32;
  uint32_t setBits = Cache::calculateTotalSetBits(totalSets); //bits to figure out which set we are in (then we match tag)
  uint32_t offsetBits = Cache::calculateTotalOffsetBits(sizePerBlock); //bits to figure out which section of the line/block we are in
  return fullAddressBits - setBits - offsetBits;
}

uint32_t Cache::hexToUL(std::string hex){
  return std::stoul(hex, nullptr, 16);
}

uint32_t Cache::getTagValue(uint32_t totalTagBits, uint32_t hex){
  return hex >> (32 - totalTagBits); //tag, set, offset
}

uint32_t Cache::getOffsetValue(uint32_t totalOffsetBits, uint32_t hex){
  uint32_t mask = (1U << totalOffsetBits) - 1; //0b100 -> 0b11
  return hex & mask;
}

uint32_t Cache::getSetValue(uint32_t totalSetBits, uint32_t totalOffsetBits, uint32_t hex){
  uint32_t hexWithoutOffset = hex >> totalOffsetBits;
  uint32_t mask = (1U << totalSetBits) - 1;
  return hexWithoutOffset & mask;
}

uint32_t Cache::calculateTotalCacheSize(uint32_t totalSets, uint32_t kAssociativity, uint32_t sizePerBlock){
  return totalSets * sizePerBlock * kAssociativity; //in bytes, 256 sets * 16 bytes/line, 4 lines/set = units of bytes
}