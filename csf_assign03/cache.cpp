#include "cache.h"

Cache::Cache(uint32_t _totalSets, uint32_t _kAssociativity, uint32_t _sizePerBlock, std::string _writeMissPolicy, std::string _writeHitPolicy, std::string _evictionPolicy)
:totalSets(_totalSets), kAssociativity(_kAssociativity), sizePerBlock(_sizePerBlock), writeMissPolicy(_writeMissPolicy), writeHitPolicy(_writeHitPolicy), evictionPolicy(_evictionPolicy){
  this -> totalSetBits = Cache::calculateTotalSetBits(this -> totalSets);
  this -> totalTagBits = Cache::calculateTotalTagBits(this -> totalSets, this -> sizePerBlock);
  this -> totalOffsetBits = Cache::calculateTotalOffsetBits(this -> sizePerBlock);

  //std::cout << "totalSets: " << totalSets << std::endl;
  //std::cout << "kAssociativity: " << kAssociativity << std::endl;
  //std::cout << "sizePerBlock: " << sizePerBlock << std::endl;
  //std::cout << "writeMissPolicy: " << writeMissPolicy << std::endl;
  //std::cout << "writeHitPolicy: " << writeHitPolicy << std::endl;
  //std::cout << "evictionPolicy: " << evictionPolicy << std::endl;
  //std::cout << "totalSetBits: " << totalSetBits << std::endl;
  //std::cout << "totalTagBits: " << totalTagBits << std::endl;
  //std::cout << "totalOffsetBits: " << totalOffsetBits << std::endl;
}

bool Cache::matchedTag(std::vector<CacheBlock> &cache, uint32_t tag) const{
  for (std::vector<CacheBlock>::iterator it = cache.begin(); it != cache.end(); it++){
    if (it -> tag == tag){
      return true;
    }
  }

  return false;
}

void Cache::cacheLoadHit(){
  this -> totalLoads++;
  this -> loadHits++;
  this -> totalCycles++;
}

void Cache::cacheLoadMiss(){
  this -> totalLoads++;
  this -> loadMisses++;
  this -> totalCycles+= 100;
}

void Cache::loadData(uint32_t address){ //RAM -> cache (cpu read)
  uint32_t tag = getTagValue(this -> totalTagBits, address);
  uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
  uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address); //even if setBits = 0 ie directMapping, still works becuase array[0]

  std::map<uint32_t, std::vector<CacheBlock>>::iterator it = this -> cacheDataStructure.find(setValue);
  if (it != cacheDataStructure.end()) { //set in map
    if (matchedTag(it -> second, tag)){ //1. cache hit
      cacheLoadHit();
    } else { //cache miss
      if (this -> cacheDataStructure[setValue].size() >= this -> kAssociativity) { //2.1 cache miss eviction
        uint32_t index = getEvictedIndex(it -> second);
        evictAndUpdateBlock(it -> second, index, tag);
        cacheLoadMiss();
      } else { //3. cache miss no eviction, no new set
        CacheBlock newBlock;
        newBlock.tag = tag;
        (it -> second).push_back(newBlock);
        cacheLoadMiss();
      }
    }
    updateTimer(it -> second, tag);
  } else { //2.2 cache miss no eviction, new set
    CacheBlock newBlock;
    newBlock.tag = tag;
    (this -> cacheDataStructure).emplace(setValue, std::vector<CacheBlock>{newBlock});
    updateTimer(this->cacheDataStructure[setValue], tag);
    cacheLoadMiss();
  }
}

void Cache::cacheStoreHit(){
  this -> totalStores++;
  this -> storeHits++;
  this -> totalCycles++;

  this -> totalLoads++;
  this -> loadHits++;
}

void Cache::cacheStoreMiss(){
  this -> totalStores++;
  this -> storeMisses++;
  this -> totalCycles+=100;

  this -> totalLoads++;
  this -> loadMisses++;
}

void Cache::storeData(uint32_t address){ //cache -> RAM (cpu write)
  uint32_t tag = getTagValue(this -> totalTagBits, address);
  uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
  uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address); //even if setBits = 0 ie directMapping, still works becuase array[0]

  std::map<uint32_t, std::vector<CacheBlock>>::iterator it = this -> cacheDataStructure.find(setValue);
  if (it != cacheDataStructure.end()) { //set in map
    if (matchedTag(it -> second, tag)){ //1. cache hit
      cacheLoadHit();
    } else { //cache miss
      if (this -> cacheDataStructure[setValue].size() >= this -> kAssociativity) { //2.1 cache miss eviction
        uint32_t index = getEvictedIndex(it -> second);
        evictAndUpdateBlock(it -> second, index, tag);
        cacheLoadMiss();
      } else { //3. cache miss no eviction, no new set
        CacheBlock newBlock;
        newBlock.tag = tag;
        (it -> second).push_back(newBlock);
        cacheLoadMiss();
      }
    }
    updateTimer(it -> second, tag);
  } else { //2.2 cache miss no eviction, new set
    CacheBlock newBlock;
    newBlock.tag = tag;
    (this -> cacheDataStructure).emplace(setValue, std::vector<CacheBlock>{newBlock});
    updateTimer(this->cacheDataStructure[setValue], tag);
    cacheLoadMiss();
  }
}

void Cache::updateTimer(std::vector<CacheBlock> &cache, uint32_t tag){
  if (this -> evictionPolicy == "fifo") {
    updateFIFO(cache);
  } else {
    updateLRU(cache, tag);
  }
}

//to update fifo or lru
void Cache::updateFIFO(std::vector<CacheBlock> &cache){
  for (std::vector<CacheBlock>::iterator it = cache.begin(); it != cache.end(); it++){
    (it -> timer)++;
  }
}

//pass a reference in instead
void Cache::updateLRU(std::vector<CacheBlock> &cache, uint32_t tag){
  for (std::vector<CacheBlock>::iterator it = cache.begin(); it != cache.end(); it++){
    if (it -> tag == tag){
      it -> timer = 0;
    } else {
      it -> timer++;
    }
  }
}

uint32_t Cache::getEvictedIndex(std::vector<CacheBlock> &cache){
  std::vector<CacheBlock>::iterator largest = cache.begin();
  for (std::vector<CacheBlock>::iterator it = cache.begin(); it != cache.end(); it++){
    if (it -> timer > largest -> timer){
      largest = it;
    }
  }
  return std::distance(cache.begin(), largest);
}

void Cache::evictAndUpdateBlock(std::vector<CacheBlock> &cache, uint32_t index, uint32_t tag){
  cache[index].tag = tag;
  if (cache[index].dirty){
    
  }
  cache[index].dirty = false;
  cache[index].timer = 0;

  //cache.erase(cache.begin() + index);
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
    
    //std:: cout << operation << " " << hex << " " << extra << std::endl;

    uint32_t address = hexToUL(hex);
    if (operation == "l"){
      loadData(address);
    } else if (operation == "s"){
      this -> totalStores++;
    }
    line = Cache::readNextLine();
    stream = std::istringstream(line);
  }

  std::cout << "Total loads: " << this -> totalLoads << std::endl;
  std::cout << "Total stores: " << this -> totalStores << std::endl;
  std::cout << "Load hits: " << this -> loadHits << std::endl;
  std::cout << "Load misses: " << this -> loadMisses << std::endl;
  std::cout << "Store hits: " << this -> storeHits << std::endl;
  std::cout << "Store misses: " << this -> storeMisses << std::endl;
  std::cout << "Total cycles: " << this -> totalCycles << std::endl;
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