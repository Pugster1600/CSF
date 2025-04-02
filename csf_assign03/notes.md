## cpp stuff
1. std::ifstream inputFile(file);
> ifstream is a class in the std namespace
> the instance of ifstream is called inputFile
> just think of it as:
ifstream inputFile(file);

std::ifstream inputFile(file);
namespace::className instanceName()
> called direct initialization

std::ifstream inputFile = std::ifstream(file);
> copy initialization
> because we create an instance in std::ifstream(file);
> then we copy contents in that address to &inputFile with the copy constructor

int main( int argc, char **argv )
> argc is just number of arguments
> argv is the double array of chars aka array of strings
> argv[0] is file name
> so 256 is argv[1]
./csim 256 4 16 write-allocate write-back fifo < gcc.trace
./csim sets k-associativity offsets*4bytes
> each section of a block is 4 bytes
> thus 16 means we have 4 sections b/c 4 sections of 4 bytes each 

the < gcc.trace redirects it to stdin of the program

static_cast allows us to safely cast at compile time without the overhead at runtime 
not as robust as dynamic cast for polymorphism
uint32_t totalSets = static_cast<uint32_t>(std::stoul(argv[1]));


void readFile(std::string file){
  std::ifstream inputFile(file);
  if (!inputFile){
    std::cerr << "Error opening file" << std::endl; //print to error console
  }
}

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

sizePerBlock = how much data a single cache line can hold

tag, index, offset

iterators are basically wrappers for the actual value returned
> they have fields with the actual value 
> it allows us to read it only but not actually write it
> or any other restrictions that the iterators have

default construcotr is the one wiht no arguents
> this is auto invoked when declared something but not directly initaliziing it like in a malloc call

 std::map<int, std::string>::iterator it;
 > means that iterator objects is part of the map scope

 maps are ordered

cache read policy
> cache hit 
> cache miss no eviction
> chache hit eviction

cache write policy
> 

bool Cache::matchedTag(std::vector<CacheBlock> &cache, uint32_t tag) const
const at the end means we are not modifying any of the params

Yes, that's correct! When you dereference an iterator and modify the value, the changes are directly reflected in the underlying data structure. This is because the iterator points to the actual elements in the container, and dereferencing the iterator gives you a reference to the element in the container.

In other words, iterators provide a way to access and modify the elements of a container. If the container holds objects or values, modifying them through the iterator modifies the original container.

for storing, this means that we are modifying the data
> so if me store the data, 

if (this -> cacheDataStructure.size() > totalSets) {
    return; 
  }
this will never happen because the totalSets = 2^set bits
> which we derived from set bits to begin with!

for write-miss, if we write-miss it with write-allocate ie we bring it to cache, that means if we load the same block later, still cache hit!
> thus storing affects loading values based on the write-miss policy indirectly (meaning write-miss should not increase load count but could indirectly if we load same block later)

but if a dirty block gets evicted, we only store once even if the value of it changes multiple times

associative: csim
	./csim 1 256 16 write-allocate write-back fifo < test.trace

set: csim
	./csim 256 4 16 write-allocate write-back fifo < test.trace

direct: csim
	./csim 256 1 16 write-allocate write-back fifo < test.trace

  if (it != cacheDataStructure.end()) { //set in map
    if (matchedTag(it -> second, tag)){ //1. cache read hit
      cacheReadHit();
    } else if (this -> cacheDataStructure[setValue].size() >= this -> kAssociativity) { //2.1 cache read miss eviction, no new set (implicit mismatch tag! ie conditions above carry)
      cacheReadMissEviction(it -> second, tag);
    } else { //2.2. cache read miss no eviction, no new set
      cacheReadMissInsert(it -> second, tag);
    }
  } else { //2.3 cache read miss no eviction, new set
    cacheReadMissNewSet(setValue, tag);
  }

    if (it != cacheDataStructure.end() && matchedTag(it -> second, tag)) {
    cacheReadHit();
  } else if (it != cacheDataStructure.end() && this -> cacheDataStructure[setValue].size() >= this -> kAssociativity) {
    cacheReadMissEviction(it -> second, tag);
  } else if (it != cacheDataStructure)

 if (matchedTag(it -> second, tag)){ //1. cache read hit
      cacheReadHit();
    } else { //2. cache miss
      if (this -> cacheDataStructure[setValue].size() == this -> kAssociativity) { //2.1 cache read miss eviction, no new set
        cacheReadMissEviction(it -> second, tag);
      } 
      else { //2.2. cache read miss no eviction, no new set
        cacheReadMissInsert(it -> second, tag);
      }
    }
    anything like this can be simplified becuause the conditoins from up top carry down just negated ie not matched tag because its else!

    more specific on top and broad on bottom!

for fifo, we keep track of when the thing was loaded only (increment when we bring data into cache either via load or store)
for lru, we keep update after every access

LRU means largest access_ts
FIFO means largest load_ts

//FIFO works for load
> LRU might not work for load

just need to implement these for store now
