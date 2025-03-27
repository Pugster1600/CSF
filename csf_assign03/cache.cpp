#include "cache.h"

//dirty block is just used for cycles calculations

Cache::Cache(uint32_t _totalSets, uint32_t _kAssociativity, uint32_t _sizePerBlock, std::string _writeMissPolicy, std::string _writeHitPolicy, std::string _evictionPolicy)
  :totalSets(_totalSets), kAssociativity(_kAssociativity), sizePerBlock(_sizePerBlock), writeMissPolicy(_writeMissPolicy), writeHitPolicy(_writeHitPolicy), evictionPolicy(_evictionPolicy), current_time(0){
  this -> totalSetBits = Cache::calculateTotalSetBits(this -> totalSets);
  this -> totalTagBits = Cache::calculateTotalTagBits(this -> totalSets, this -> sizePerBlock);
  this -> totalOffsetBits = Cache::calculateTotalOffsetBits(this -> sizePerBlock);

  for (uint32_t i = 0; i < this -> totalSets; i++){
    (this -> cacheDataStructure).emplace(i, std::vector<CacheBlock>(kAssociativity));
    for (uint32_t j = 0; j < kAssociativity; j++){
      this->cacheDataStructure[i][j].access_ts = 0;
      this->cacheDataStructure[i][j].load_ts = 0;
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
  /*for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if (it -> tag == tag && it -> valid){
      return true;
    }
  }
  return false;
  */
  
  
  uint32_t index = 0;
    for (uint32_t i = 0; i < kAssociativity; i++) {
        if (set[i].tag == tag && set[i].valid) {
	  return true;
        }
    }
    return false;
    
}

void Cache::cacheLoadHitUpdateStats(){
  this -> totalLoads++;
  this -> loadHits++;
  //this -> totalCycles++;
}

void Cache::cacheLoadMissUpdateStats(){
  this -> totalLoads++;
  this -> loadMisses++;
  //this -> totalCycles += 100;
}

void Cache::loadData(uint32_t address){ //RAM -> cache (cpu read)
  uint32_t tag = getTagValue(this -> totalTagBits, address);
  uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
  uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address); //even if setBits = 0 ie directMapping, still works becuase array[0]

  std::vector<CacheBlock>& set = cacheDataStructure[setValue];
  uint32_t cyclesPerBlock = 100 * (sizePerBlock / 4); // Cost to transfer a full block
  
  if (matchedTag(set, tag)){ //1. cache read hit
    uint32_t index = getIndexOfBlock(set, tag);
    if (this -> evictionPolicy == "lru"){
    set[index].access_ts = current_time++;
    }
    cacheLoadHitUpdateStats();
    totalCycles++;
  } else if (getNotValidLineIndex(set) == (kAssociativity)) { //2.1 cache read miss eviction
    uint32_t index;
    if (evictionPolicy == "lru") {
      index = getOldest(set);
    } else {
      index = getOldestFIFO(set);
    }
    evictAndUpdateBlock(set, index, tag);
    if (evictionPolicy == "lru") {
      set[index].access_ts = current_time++; // Incremented here
    }
    cacheLoadMissUpdateStats();
    totalCycles+=cyclesPerBlock;
  } else { //2.2 cache read miss, no eviction
     uint32_t index = getNotValidLineIndex(set);
     set[index].valid = true;
     set[index].tag = tag;
     if (evictionPolicy == "lru") {
       set[index].access_ts = current_time++; // Incremented here
     } else if (evictionPolicy == "fifo") {
       set[index].load_ts = current_time++; // Optional: for FIFO
     }
     cacheLoadMissUpdateStats();
     totalCycles+=cyclesPerBlock+1;
    }
}


uint32_t Cache::getIndexFromTimer(std::vector<CacheBlock> &set, uint32_t timer) {
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if ((this -> evictionPolicy == "fifo") && (it -> load_ts == timer)) {
      return std::distance(set.begin(), it);
    } else if ((this -> evictionPolicy == "lru") && (it -> access_ts == timer)) {
      return std::distance(set.begin(), it);
    }
  }
  return this -> kAssociativity;
}

uint32_t Cache::getNotValidLineIndex(std::vector<CacheBlock> &set) {
  for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
    if ((it -> valid) == false){
      std::cout << it -> valid << std::endl;
      return std::distance(set.begin(), it);
    }
  }

  return this -> kAssociativity; //this returns for some reason
}

void Cache::cacheStoreHitUpdateStats(){
  this -> totalStores++;
  this -> storeHits++;
  //this -> totalCycles++;
}

void Cache::cacheStoreMissUpdateStats(){
  this -> totalStores++;
  this -> storeMisses++;
  //this -> totalCycles += 100;
}

void Cache::storeData(uint32_t address){ //cache -> RAM (cpu write)
  uint32_t tag = getTagValue(this -> totalTagBits, address);
  uint32_t offset = getOffsetValue(this -> totalOffsetBits, address);
  uint32_t setValue = getSetValue(this -> totalSetBits, this -> totalOffsetBits, address); //even if setBits = 0 ie directMapping, still works becuase array[0]
  uint32_t cyclesPerBlock = 100 * (sizePerBlock / 4); // Cost to transfer a full block

  std::vector<CacheBlock>& set = this->cacheDataStructure[setValue]; // simplified this
  if (matchedTag(set, tag)){ // if matchedTag returns true then block was found in set
    uint32_t index = getIndexOfBlock(set, tag);
    if (this -> evictionPolicy == "lru") {
      set[index].access_ts = current_time++;
    }
    
    if(this -> writeHitPolicy == "write-back"){
      set[index].dirty = true;
      totalCycles+= 1;
    }
    else {
      totalCycles+= 101;
    }
    cacheStoreHitUpdateStats();
  } else if (this -> writeMissPolicy == "write-allocate"){
    
    uint32_t index = getNotValidLineIndex(set);
    if (index == kAssociativity) { // Set full, evict
      if (evictionPolicy == "lru") {
	index = getOldest(set);
      }
      else {
	index = getOldestFIFO(set);
      }
      //index = getOldest(set);
      evictAndUpdateBlock(set, index, tag);
      if (evictionPolicy == "lru") {
      set[index].access_ts = current_time++; // Set and increment
      }
      else {
	set[index].load_ts = current_time++;
      }
      if (this -> writeHitPolicy == "write-back") {
	set[index].dirty = true;
	totalCycles+=cyclesPerBlock+1;
      }
      else {
	totalCycles+=cyclesPerBlock;
      }
      cacheStoreMissUpdateStats();
      
      } else { // Insert into invalid slot
         set[index].tag = tag;
         set[index].valid = true;
	 if (evictionPolicy == "lru") {
	   set[index].access_ts = current_time++; // Set and increment
	 }
	 else {
	   set[index].load_ts = current_time++;
	 }
	 if (this -> writeHitPolicy == "write-back") {
	   set[index].dirty = true;
	   totalCycles++;
	 }
	 else {
	   totalCycles+=101;
	 }
	 cacheStoreMissUpdateStats();
  }
	 
  } else { // 3. No-write-allocate
        // No cache update, just stats
        cacheStoreMissUpdateStats();
	      totalCycles+=100;
  }
  
    
}


uint32_t Cache::getOldest(std::vector<CacheBlock>& set) { // finds least recently used block
    uint32_t min_ts = UINT32_MAX; // Start with max value
    uint32_t index = 0;
    for (uint32_t i = 0; i < kAssociativity; i++) {
        if (set[i].valid && set[i].access_ts < min_ts) {
            min_ts = set[i].access_ts;
            index = i;
        }
    }
    return index;
}

uint32_t Cache::getOldestFIFO(std::vector<CacheBlock>& set) {
  uint32_t min_ts = UINT32_MAX;
  uint32_t index = 0;
  for (uint32_t i = 0; i < kAssociativity; i++) {
    if (set[i].valid && set[i].load_ts < min_ts) {
      min_ts = set[i].load_ts;
      index = i;
    }
  }
  return index;
}

uint32_t Cache::getIndexOfBlock(std::vector<CacheBlock> &set, uint32_t tag){
  //for (std::vector<CacheBlock>::iterator it = set.begin(); it != set.end(); it++){
  /* if (it -> tag == tag){
      return std::distance(set.begin(), it);
    }
    } */
  
  for (uint32_t i = 0; i < kAssociativity; i++) {
    if (set[i].tag == tag) {
      return i;
    }
  }



  return 0; //will never happen because of usuage
}



uint32_t Cache::getLargestValidLineIndex(std::vector<CacheBlock> &set){
  
  uint32_t largestIndex = 0;
    
  if (this->evictionPolicy == "fifo") {
      for (uint32_t i = 1; i < kAssociativity; i++) {
         if ((set[i].load_ts > set[largestIndex].load_ts) && set[i].valid) {
            largestIndex = i;
         }
      }
  } else if (this->evictionPolicy == "lru") {
      for (uint32_t i = 1; i < kAssociativity; i++) {
         if ((set[i].access_ts > set[largestIndex].access_ts) && set[i].valid) {
             largestIndex = i;
         }
      }
  }
    
  return largestIndex;
}

void Cache::evictAndUpdateBlock(std::vector<CacheBlock> &set, uint32_t index, uint32_t tag){
  if (set[index].dirty == true) {
    this -> totalCycles += (100*(sizePerBlock/4));
  }

  set[index].tag = tag;
  set[index].dirty = false;
  set[index].valid = true;
  if (this -> evictionPolicy == "fifo") {
    set[index].load_ts = current_time++;
  } else {
    set[index].access_ts = current_time++;
  }
}

void Cache::mainLoop(){
  
  std::string line;
  while ((line = readNextLine()) != "") {
    std::istringstream stream(line);
    std::string operation, hex;
    if (!(stream >> operation >> hex)) {
      std::cerr << "Invalid input: " << line << std::endl;
      continue;
    }
    if (operation != "l" && operation != "s") {
      std::cerr << "Unknown operation: " << operation << std::endl;
      continue;
    }
    try {
      uint32_t address = hexToUL(hex);
      if (operation == "l") loadData(address);
      else storeData(address);
    } catch (const std::exception& e) {
      std::cerr << "Invalid address: " << hex << std::endl;
    }
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
  //uint32_t sectionsPerBlock = sizePerBlock / 4;
  return static_cast<uint32_t>(log2(sizePerBlock));  //if each line/block has 16 bytes, then we have 4 offsets
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
