PCNAME	:= $(shell uname -n)
OSNAME	:= $(shell uname)
ifeq ($(PCNAME), nova)
	CC	:= g++-8.3.0
else ifeq ($(OSNAME), Darwin)
	CC	:= g++-8
else
	CC	:= g++
endif

CFLAGS		:= -std=c++17 -Wall -Wextra -Werror -pedantic-errors -fPIC

LIB_TARGET1	:= LeastFreqSearchMultiMarks.so 
LIB_TARGET2 := NaiveMultiMarks.so 

LIB_OBJS1	:= LeastFreqSearchMultiMarks.o
LIB_OBJS2 	:= NaiveMultiMarks.o 


ifeq ($(OSNAME), Darwin)
	LIB_FLAGS	:= -shared -undefined dynamic_lookup
else 
	LIB_FLAGS	:= -shared
endif
LIB_OBJS 	:= LeastFreqSearchMultiMarks.so NaiveMultiMarks.so

.PHONY: clean

all: algorithm_lib1 algorithm_lib2

algorithm_lib1: $(LIB_TARGET1)
algorithm_lib2: $(LIB_TARGET2)


$(LIB_TARGET1): $(LIB_OBJS1)
	$(CC) $(LIB_OBJS1) -o $@ $(LIB_FLAGS)

$(LIB_TARGET2): $(LIB_OBJS2)
	$(CC) $(LIB_OBJS2) -o $@ $(LIB_FLAGS)


SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)

$(OBJS): %.o : %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXE_TARGET)

-include $(DEPS)
