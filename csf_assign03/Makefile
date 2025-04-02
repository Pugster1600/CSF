CXX = g++
CXXFLAGS = -g -Wall -pedantic -std=c++17

# Add any additional source files here
SRCS = main.cpp
SRCS += cache.cpp
OBJS = $(SRCS:.cpp=.o)

# When submitting to Gradescope, submit all .cpp and .h files,
# as well as README.txt
FILES_TO_SUBMIT = $(shell ls *.cpp *.h README.txt Makefile cache.cpp cache.h 2> /dev/null)

# Rule for compiling .cpp to .o
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

# Executable target
csim : $(OBJS)
	$(CXX) -o $@ $+

# Target to create a solution.zip file you can upload to Gradescope
.PHONY: solution.zip
solution.zip :
	zip -9r $@ $(FILES_TO_SUBMIT)

test : csim
	./csim 256 4 16 write-allocate write-back fifo < gcc.trace
# ./csim 1 8192 16 write-allocate write-through lru < gcc.trace
# ./csim 1 8192 16 write-allocate write-back lru < gcc.trace
#set args 1 8192 16 write-allocate write-through lru < gcc.trace
sanity: csim
	./csim 256 4 16 write-allocate write-back fifo < test.trace
#./csim 1 1 4 write-allocate write-back fifo < test.trace
associative: csim
	./csim 1 256 16 write-allocate write-back fifo < test.trace

set: csim
	./csim 256 4 16 write-allocate write-back fifo < test.trace

direct: csim
	./csim 256 1 16 write-allocate write-back fifo < test.trace

gdb : csim
	gdb ./csim

#set args 256 4 16 write-allocate write-back fifo < gcc.trace
#set args 1 1 4 write-allocate write-back fifo < test.trace
# Generate header file dependencies
#valgrind --leak-check=full --show-leak-kinds=all ./csim 256 4 16 write-allocate write-back fifo < gcc.trace
depend :
	$(CXX) $(CXXFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

clean :
	rm -f csim *.o
include depend.mak
