//#include "cache.h"

//dirty block is just used for cycles calculations

Cache::Cache(uint32_t _totalSets, uint32_t _kAssociativity, uint32_t _sizePerBlock, std::string _writeMissPolicy, std::string _writeHitPolicy, std::string _evictionPolicy)
:totalSets(_totalSets), kAssociativity(_kAssociativity), sizePerBlock(_sizePerBlock), writeMissPolicy(_writeMissPolicy), writeHitPolicy(_writeHitPolicy), evictionPolicy(_evictionPolicy){
  this -> totalSetBits = Cache::calculateTotalSetBits(this -> totalSets);
  this -> totalTagBits = Cache::calculateTotalTagBits(this -> totalSets, this -> sizePerBlock);
  this -> totalOffsetBits = Cache::calculateTotalOffsetBits(this -> sizePerBlock);

  for (uint32_t i = 0; i < this -> totalSets; i++){
    (this -> cacheDataStructure).emplace(i, std::vector<CacheBlock>(kAssociativity));
    for (uint32_t j = 0; j < kAssociativity; j++){
      this->cacheDataStructure[i][j].access_ts = counter;
      this->cacheDataStructure[i][j].load_ts = counter;
      this->cacheDataStructure[i][j].valid = false;
    }
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

bool Cache::matchedTag(std::vector<CacheBlock> &set, uint32_t tag){
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if (it -> tag == tag && it -> valid){
      return true;
    }
  }
  return false;
}

void Cache::cacheLoadHitUpdateStats(){
  this -> totalLoads++;
  this -> loadHits++;
  this -> totalCycles++;
}

void Cache::cacheLoadMissUpdateStats(){
  this -> totalLoads++;
  this -> loadMisses++;
  this -> totalCycles += 100;
}

void Cache::loadData(uint32_t address){ //RAM -> cache (cpu read)
  uint32_t tag = getTagValue(this -> totalTagBits, address);
  uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
  uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address); //even if setBits = 0 ie directMapping, still works becuase array[0]

  std::map<uint32_t, std::vector<CacheBlock>>::iterator it = this -> cacheDataStructure.find(setValue);
  
  if (matchedTag(it -> second, tag)){ //1. cache read hit
    cacheReadHit(it -> second, tag);
  } else if (getNotValidLineIndex(this -> cacheDataStructure[setValue]) == (kAssociativity)) { //2.1 cache read miss eviction
    cacheReadMissEviction(it -> second, tag);
  } else if (getNotValidLineIndex(this -> cacheDataStructure[setValue]) != (kAssociativity)) { //2.2 cache read miss, no eviction
    cacheReadMissInsert(it -> second, tag);
  }
}

void Cache::cacheReadHit(std::vector<CacheBlock> &set, uint32_t tag){ //
  uint32_t index = getIndexOfBlock(set, tag);

  if (this -> evictionPolicy == "lru") { //cache hit only update for LRU
    updateLRULoad(set, set[index].access_ts);
    set[index].access_ts = 0;
  } 

  cacheLoadHitUpdateStats();
}

void Cache::cacheReadMissEviction(std::vector<CacheBlock> &set, uint32_t tag){
  uint32_t index = getLargestValidLineIndex(set); //largest index will be == k, so it will reset to 0
  if (this -> evictionPolicy == "fifo") {
    updateFIFOLoad(set, this -> kAssociativity); //this will just increment them all
  } else if (this -> evictionPolicy == "lru"){
    updateLRULoad(set, this -> kAssociativity); //this will also just increment them all
  }
  evictAndUpdateBlock(set, index, tag);
  cacheLoadMissUpdateStats();
}

void Cache::cacheReadMissInsert(std::vector<CacheBlock> &set, uint32_t tag){
  uint32_t index = getNotValidLineIndex(set);
  //std::cout << index << std::endl;
  if (this -> evictionPolicy == "fifo") {
    updateFIFOLoad(set, this -> kAssociativity);
    set[index].load_ts = 0;
  } else if (this -> evictionPolicy == "lru") {
    updateLRULoad(set, this -> kAssociativity);
    set[index].access_ts = 0;
  }

  set[index].valid = true;
  set[index].tag = tag;
  cacheLoadMissUpdateStats();
}

uint32_t Cache::getNotValidLineIndex(std::vector<CacheBlock> &set) {
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if (!(it -> valid)){
      //std::cout << it -> valid << std::endl;
      return std::distance(set.begin(), it);
    }
  }

  return this -> kAssociativity; //this returns for some reason
}

void Cache::cacheStoreHitUpdateStats(){
  this -> totalStores++;
  this -> storeHits++;
  this -> totalCycles++;
}

void Cache::cacheStoreMissUpdateStats(){
  this -> totalStores++;
  this -> storeMisses++;
  this -> totalCycles += 100;
}

void Cache::storeData(uint32_t address){ //cache -> RAM (cpu write)
  uint32_t tag = getTagValue(this -> totalTagBits, address);
  uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
  uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address); //even if setBits = 0 ie directMapping, still works becuase array[0]

  std::map<uint32_t, std::vector<CacheBlock>>::iterator it = this -> cacheDataStructure.find(setValue);
  if (matchedTag(it -> second, tag) && this -> writeHitPolicy == "write-back"){ //cache write hit, write back (tag in set) -> only write back has dirty 
    //1. if lru, do the lru update policy, if fifo dont do anything
    cacheWriteHitWriteBack(it -> second, tag);
  } else if (matchedTag(it -> second, tag) && (this -> writeHitPolicy == "write-through")) { //cache write hit, write through
    //2. if lru, do the lru update policy, if fifo dont do anything
    cacheWriteHitWriteThrough(it -> second, tag);
    cacheStoreHitUpdateStats();
  } else if (this -> writeMissPolicy == "write-allocate" && getNotValidLineIndex(this -> cacheDataStructure[setValue]) == (kAssociativity)){
    //3. if lru, do the lru update policy, if fifo update fifo
    cacheWriteMissWriteAllocateEviction(it -> second, tag);
  } else if (this -> writeMissPolicy == "write-allocate") {
    //4. if lru, do the lru update policy, if fifo update fifo
    cacheWriteMissWriteAllocateInsert(it -> second, tag);
  } else if (this -> writeMissPolicy == "no-write-allocate") {
    //5. if lru, do the lru update policy, if fifo dont do anything
    cacheWriteMissNoWriteAllocate(it->second, tag);
  }
}

void Cache::cacheWriteHitWriteBack(std::vector<CacheBlock> &set, uint32_t tag){
  uint32_t index = getIndexOfBlock(set, tag);
  if (this -> evictionPolicy == "lru") {
    updateLRUStore(set, set[index].access_ts);
    set[index].access_ts = 0;
  } 
  
  //else if (this -> evictionPolicy == "fifo") {
  //  updateFIFOStore(set, set[index].load_ts);
  //  set[index].load_ts = 0;
  //} //for write back since we are rewritting i think we need to update fifo
  set[index].dirty = true;
  cacheStoreHitUpdateStats();
}

void Cache::cacheWriteHitWriteThrough(std::vector<CacheBlock> &set, uint32_t tag){
  uint32_t index = getIndexOfBlock(set, tag);
  if (this -> evictionPolicy == "lru") { //2. if lru, do the lru update policy, if fifo dont do anything
    updateLRUStore(set, set[index].access_ts);
    set[index].access_ts = 0;
  } 
  
  //else if (this -> evictionPolicy == "fifo") {
  //  updateFIFOStore(set, set[index].load_ts);
  //  set[index].load_ts = 0;
  //}
  //for write through since we are rewritting i think we need to update fifo
  cacheStoreHitUpdateStats();
}

void Cache::cacheWriteMissWriteAllocateEviction(std::vector<CacheBlock> &set, uint32_t tag){
  uint32_t index = getLargestValidLineIndex(set);
  if (this -> evictionPolicy == "fifo") {
    updateFIFOStore(set, this -> kAssociativity);
  } else if (this -> evictionPolicy == "lru") {
    updateLRUStore(set, this -> kAssociativity);
  }
  evictAndUpdateBlock(set, index, tag);
  cacheStoreMissUpdateStats(); 
}

void Cache::cacheWriteMissWriteAllocateInsert(std::vector<CacheBlock> &set, uint32_t tag){
  uint32_t index = getNotValidLineIndex(set);
  //std::cout << index << std::endl;
  if (this -> evictionPolicy == "fifo") {
    updateFIFOStore(set, this -> kAssociativity);
    set[index].load_ts = 0;
  } else if (this -> evictionPolicy == "lru") {
    updateLRUStore(set, this -> kAssociativity); //I THINK THIS IS WRONG!!
    set[index].access_ts = 0;
  }
  set[index].valid = true;
  set[index].tag = tag;
  cacheStoreMissUpdateStats();
}

void Cache::cacheWriteMissNoWriteAllocate(std::vector<CacheBlock> &set, uint32_t tag){
  cacheStoreMissUpdateStats();
  //5. if lru, do the lru update policy, if fifo dont do anything
  //uint32_t index = getIndexOfBlock(it -> second, tag);
  //if (this -> evictionPolicy == "lru") { //2. if lru, do the lru update policy, if fifo dont do anything
  //  updateTimerStore(this->cacheDataStructure[setValue], this->cacheDataStructure[setValue][index].access_ts);
  //  (it->second)[index].access_ts = 0;
  //}
}


uint32_t Cache::getIndexOfBlock(std::vector<CacheBlock> &set, uint32_t tag){
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if (it -> tag == tag){
      return std::distance(set.begin(), it);
    }
  }

  return 0; //will never happen because of usuage
}

//to update fifo or lru -> all valid get incremented
void Cache::updateFIFOLoad(std::vector<CacheBlock> &set, uint32_t load_ts){
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if ((it -> valid) && (it -> load_ts < load_ts)){
      it -> load_ts++; //largest non valid will always be 0 (largest valid if overflows will always be 0)
    }
  }
}

//pass a reference in instead -> all behind get incremented
void Cache::updateLRULoad(std::vector<CacheBlock> &set, uint32_t access_ts){
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if ((it -> valid) && (it -> access_ts < access_ts)){// && ((it -> access_ts <= access_ts) || (it -> access_ts == 0))
      it -> access_ts++;
    } 
  }
}

//--------------------------------------------------------------------

//to update fifo or lru
void Cache::updateFIFOStore(std::vector<CacheBlock> &set, uint32_t load_ts){
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if ((it -> valid) && (it -> load_ts < load_ts)) {
      (it -> load_ts)++;
    }
  }
}

//pass a reference in instead
void Cache::updateLRUStore(std::vector<CacheBlock> &set, uint32_t access_ts){
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if ((it -> valid)  && (it -> access_ts < access_ts)){ //&& ((it -> access_ts <= access_ts) || (it -> access_ts == 0))
      it -> access_ts++;
    } 
  }
}

uint32_t Cache::getLargestValidLineIndex(std::vector<CacheBlock> &set){
  std::vector<CacheBlock>::iterator largest = set.begin();
  if (this -> evictionPolicy == "fifo") {
    for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
      if ((it -> load_ts > largest -> load_ts) && it -> valid){
        largest = it;
      }
    }
  } else if (this -> evictionPolicy == "lru") {
    for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
      if ((it -> access_ts > largest -> access_ts) && it -> valid){
        largest = it;
      }
    }
  }
  
  return std::distance(set.begin(), largest);
}

void Cache::evictAndUpdateBlock(std::vector<CacheBlock> &set, uint32_t index, uint32_t tag){
  if (set[index].dirty == true) {
    this -> totalCycles+=100;
  }

  set[index].tag = tag;
  set[index].dirty = false;
  set[index].valid = true;

  if (this -> evictionPolicy == "fifo") {
    set[index].load_ts = 0;
  } else if (this -> evictionPolicy == "lru"){
    set[index].access_ts = 0;
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
    
    //std:: cout << operation << " " << hex << " " << extra << std::endl;

    uint32_t address = hexToUL(hex);

    uint32_t tag = getTagValue(this -> totalTagBits, address);
    uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
    uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address);

    //std::cout << "tag: " << tag << std::endl;
    //std::cout << "offset: " << offset << std::endl;
    //std::cout << "setValue: " << setValue << std::endl;

    if (operation == "l"){
      loadData(address);
    } else if (operation == "s"){
      storeData(address);
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

//./csim 1 8192 16 write-allocate write-through lru