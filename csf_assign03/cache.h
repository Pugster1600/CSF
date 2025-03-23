#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>
#include <map>
#include <sstream>

typedef struct CacheBlock{
  uint32_t tag = 0;
  uint32_t timer = 0;
  bool dirty = false;
} CacheBlock;

//NOTE: we dont need to track what values are in set becuase by definition it is derived from the totalSets ie 8 bits thus we can have max 256 anyways
//same with offset
class Cache{
  public:
    Cache(uint32_t _totalSets, uint32_t _kAssociativity, uint32_t _sizePerBlock, std::string _writeMissPolicy, std::string _writeHitPolicy, std::string _evictionPolicy);

    void mainLoop();

  private:
    //line/set/block# (uint32_t): tag (array of tags = kAssociativity) : timer (either fifo or lru)
    //#keys = sets, #arraySlots = #kAssociativity
    //std::map<uint32_t, std::vector<uint32_t>> cacheDataStructure; //fully associative has 1 key aka 1 set, direct mapping has vector size = 1
    std::map<uint32_t, std::vector<CacheBlock>> cacheDataStructure;
    uint32_t totalSets;
    uint32_t kAssociativity;
    uint32_t sizePerBlock;
    std::string writeMissPolicy; //write-allocate (allocate in cache) or no-write-allocate (write-allocate means bring to cache then update, no-write means straight to RAM)
    std::string writeHitPolicy; //write-through (through to RAM) or write-back (write through means always update RAM, write-back means update RAM when kicked)
    std::string evictionPolicy; 
    uint32_t totalSetBits;
    uint32_t totalTagBits;
    uint32_t totalOffsetBits;

    uint32_t totalLoads = 0;
    uint32_t loadHits = 0;
    uint32_t loadMisses = 0;

    uint32_t totalStores = 0;
    uint32_t storeHits = 0;
    uint32_t storeMisses = 0;
    
    uint32_t totalCycles = 0;

    std::string readNextLine();

    void loadData(uint32_t address); //cache read policy (read from RAM if miss)
    void cacheLoadHit();
    void cacheLoadMiss();

    void storeData(uint32_t address); //cache write policy (store to RAM if miss)
    void cacheStoreHit();
    void cacheStoreMiss();
    void noWriteAllocate();
    void writeAllocate();
    void writeThrough(); //cache write hit -> always update in RAM and cache
    void writeBack(); //cache write hit -> update just in cache, mark as dirty then when evicted write back

    void updateTimer(std::vector<CacheBlock> &cache, uint32_t tag);
    void updateFIFO(std::vector<CacheBlock> &cache);
    void updateLRU(std::vector<CacheBlock> &cache, uint32_t tag);

    uint32_t getEvictedIndex(std::vector<CacheBlock> &cache);
    void evictAndUpdateBlock(std::vector<CacheBlock> &cache, uint32_t index, uint32_t tag);

    bool matchedTag(std::vector<CacheBlock> &cache, uint32_t tag) const;

    uint32_t calculateTotalSetBits(uint32_t totalSets);
    uint32_t calculateTotalOffsetBits(uint32_t sizePerBlock);
    uint32_t calculateTotalTagBits(uint32_t totalSets, uint32_t sizePerBlock);
    uint32_t hexToUL(std::string hex);
    uint32_t getTagValue(uint32_t totalTagBits, uint32_t hex);
    uint32_t getOffsetValue(uint32_t totalOffsetBits, uint32_t hex);
    uint32_t getSetValue(uint32_t totalSetBits, uint32_t totalOffsetBits, uint32_t hex);
    uint32_t calculateTotalCacheSize(uint32_t totalSets, uint32_t kAssociativity, uint32_t sizePerBlock);
  };

#endif 