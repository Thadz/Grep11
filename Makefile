CC = g++
CFLAGS = -std=c++11 -Wall -g

SRCS = nfa.cpp nfa_api.cpp main.cpp

OBJS = $(SRCS:.c=.o)

MAIN = grep

.PHONY: clean

all: $(MAIN)
	@echo simple grep has been compiled

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN)
