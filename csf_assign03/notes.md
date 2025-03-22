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