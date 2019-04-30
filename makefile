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

EXE_TARGET	:= ex3
EXE_FLAGS	:= -pthread -rdynamic -ldl -lstdc++fs
EXE_OBJS	:= main.o AlgorithmRegistration.o

LIB_TARGET	:= Algorithm.so
LIB_FLAGS	:= -shared -undefined dynamic_lookup
LIB_OBJS	:= Algorithm.o 

.PHONY: clean

all: maze_tournament maze_lib

maze_tournament: $(EXE_TARGET)

maze_lib: $(LIB_TARGET)

$(EXE_TARGET): $(EXE_OBJS)
	$(CC) $(EXE_OBJS) -o $@ $(EXE_FLAGS)

$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(LIB_OBJS) -o $@ $(LIB_FLAGS)

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)

$(OBJS): %.o : %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXE_TARGET) $(LIB_TARGET)

-include $(DEPS)