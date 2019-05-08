PCNAME	:= $(shell uname -n)
OSNAME	:= $(shell uname)
ifeq ($(PCNAME), nova)
	CC	:= g++-5.3.0
else ifeq ($(OSNAME), Darwin)
	CC	:= g++-8
else
	CC	:= g++
endif

CFLAGS		:= -std=c++17 -Wall -Wextra -Werror -pedantic-errors -fPIC

EXE_TARGET	:= match
EXE_FLAGS	:= -pthread -rdynamic -ldl -lstdc++fs
EXE_OBJS	:= main.o AlgorithmRegistration.o MatchManager.o Parser.o Maze.o GameManager.o

LIB_TARGET1	:= Algorithm.so 
LIB_TARGET2 := LeastFreqSearch.so 
LIB_FLAGS	:= -shared -undefined dynamic_lookup
LIB_OBJS1	:= Algorithm.o
LIB_OBJS2 	:= LeastFreqSearch.o 
LIB_OBJS 	:= LeastFreqSearch.so Algorithm.so

all: maze_tournament algorithm_lib1 algorithm_lib2
libs: algorithm_lib1 algorithm_lib2

maze_tournament: $(EXE_TARGET)

algorithm_lib1: $(LIB_TARGET1)
algorithm_lib2: $(LIB_TARGET2)


$(EXE_TARGET): $(EXE_OBJS)
	$(CC) $(EXE_OBJS) -o $@ $(EXE_FLAGS)

$(LIB_TARGET1): $(LIB_OBJS1)
	$(CC) $(LIB_OBJS1) -o $@ $(LIB_FLAGS)

$(LIB_TARGET2): $(LIB_OBJS2)
	$(CC) $(LIB_OBJS2) -o $@ $(LIB_FLAGS)

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)

$(OBJS): %.o : %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(EXE_TARGET) $(LIB_OBJS)

-include $(DEPS)
	
