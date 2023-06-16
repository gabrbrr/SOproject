CC=gcc
CCOPTS= -w --std=gnu99 -Wall 

OBJS=MMU.o

HEADERS=MMU.h

BINS=test_progieto

.phony: clean all


all:	$(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

test_progieto:	test_progieto.c $(OBJS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(OBJS) $(BINS)