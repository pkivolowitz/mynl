CFLAGS	= -Wall -g -std=c++11
LFLAGS	= 
CC		= g++

srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

mynl: $(objs)
	$(CC) $^ -o $@ $(LFLAGS)

%.o: %.cpp
	$(CC) -MMD -MP $(CFLAGS) -c $< -o $@

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(objs) $(deps) a.out mynl core

-include $(deps)
