#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>

/*
background:
format is 
s 0x1fffff50 1
l 0x1fffff58 1
s 0x3004d980 9

1. s,l 
s meaning cache is storing to memory (write policy)
l meaning cache is loading from memory (read policy)
all in cache persective, remember

2. 0xdeadbeef is just the address

3. the final value, like 1, 9 etc we can ignore for now

each load/store only accesses at most 4 bytes of data

cache will be configured with a few attributes
1. sets in cache (power of 2)
2. blocks in each set (power of 2)
3. bytes in each block/line (power of 2, at least 4)
4. cache write miss
write-allocate (move into cache then update in cache) 
no-write-allocate (write straight to RAM)
5. cache write hit
write-through (update in cache and RAM for each write)
write-back (only write when dirty block evicted)
6. lru or fifo evication

remember (memory and RAM used interchangeably here):
for storing to memory in write-allocate cache miss
meaning that we wanted to write to memory but the memory block is not in cache
> we bring the memory to cache first
> then we update the memory in cache
> then we write it back to memory

for no-write-allocate
> a cache miss during a store does not modify the cache because we are writing
straight to RAM not cache

it does not make sense for no-write-allocate and write-back because
if we have a write miss, we write straight to RAM and never store it in cache
thus write-back never gets used as the block never gets dirty since it was never in cache!

before running, program should check if
block size is not a power of 2
number of sets not a power of 2
block size less than 4
write-backa nd no-write allocate were specified

ideas of data structure
1. direct mapped
> can just be a dictionary
line/index number as the key
tag as the value
but we also need to keep track of dirtiness

NOTE: the assumption is that if say sizePerBlock = 16, we have 4 offsets
meaning that 2 bits of 32 are for offsets

256, 4, 16
uint32_t totalSets, uint32_t kAssociativity, uint32_t sizePerBlock

if totalSets = 256, this means we have 2^8 
meanign 8 bits of 32 are for line/index
the remaining 10 bits are for tag

check if directMapped by seeing if kAssociativity = 1
meaning that totalLines = totalSets * kAssociativity

totalLines = totalSets
kAssociativity = 1

2. associative mapped
> can also just be a dictionary
check if sets = 1

totalLines = kAssociativity
totalSets = 1
*/


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

class directMappedCache{
public:
  directMappedCache(uint32_t totalSets, uint32_t kAssociativity, uint32_t){

  }
private:

};
int main( int argc, char **argv ) {
  // TODO: implement
  if (argc != 8) { //argc == 7 btw
    std::cout << argc << std::endl;
    std::cout << argv[0] << std::endl;
    std::cout << argv[1] << std::endl;
    std::cout << argv[2] << std::endl;
    std::cout << argv[3] << std::endl;
    std::cout << argv[4] << std::endl;
    std::cout << argv[5] << std::endl;
    std::cout << argv[6] << std::endl;
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
  return 0;
}
