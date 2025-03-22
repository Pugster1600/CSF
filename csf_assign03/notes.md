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