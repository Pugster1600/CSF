CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu11

SRCS = uint256.c uint256_tests.c tctest.c
OBJS = $(SRCS:%.c=%.o)

all : uint256_tests

uint256_tests : $(OBJS)
	$(CC) -o $@ $(OBJS)

clean :
	rm -f $(OBJS) uint256_tests depend.mak

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

valgrind: uint256_tests
	valgrind --leak-check=full --show-leak-kinds=all ./uint256_tests

gdb: uint256_tests
	gdb ./uint256_tests

include depend.mak
