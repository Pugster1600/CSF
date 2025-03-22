#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>
#include <map>
#include <sstream>

class Cache{
  public:
    Cache(uint32_t _totalSets, uint32_t _kAssociativity, uint32_t _sizePerBlock, std::string _writeMissPolicy, std::string _writeHitPolicy, std::string _evictionPolicy);

    void mainLoop();

  private:
    //line/set/block# (uint32_t): tag (array of tags = kAssociativity) : timer (either fifo or lru)
    std::map<uint32_t, std::vector<uint32_t>> cacheDataStructure; //fully associative has 1 key aka 1 set, direct mapping has vector size = 1
    uint32_t totalSets;
    uint32_t kAssociativity;
    uint32_t sizePerBlock;
    std::string writeMissPolicy;
    std::string writeHitPolicy;
    std::string evictionPolicy;
    uint32_t totalSetBits;
    uint32_t totalTagBits;
    uint32_t totalOffsetBits;

    std::string readNextLine();

    void loadData(uint32_t address); //cache read policy (read from RAM if miss)

    void storeData(uint32_t address); //cache write policy (store to RAM if miss)

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