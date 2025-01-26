CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu11

SRCS = uint256.c uint256_tests.c tctest.c
OBJS = $(SRCS:%.c=%.o)

all : uint256_tests

OWN_SRCS = uint256.c tests.c
OWN_OBJS = $(OWN_SRCS:%.c=%.o)

tests : $(OWN_OBJS)
	$(CC) -o $@ $(OWN_OBJS) 
#represents the target aka own_tests ie the resulting linking + compiling is own_test
#if i changed own_tests to tests, the resulting would be tests

uint256_tests : $(OBJS)
	$(CC) -o $@ $(OBJS)

clean :
	rm -f $(OBJS) uint256_tests depend.mak $(OWN_OBJS)

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@



include depend.mak
