.PHONY:clean
CC=gcc
CFLAGS=-Wall -g
ALL=main
all:$(ALL)
OBJS=threadpool.o main.o condition.o
.o.c:
	$(CC) $(CFLAGS) -c $< 

main:$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lrt

clean:
	rm -f  $(ALL) *.o 
