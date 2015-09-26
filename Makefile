CC=gcc
CFLAGS = -c -Wall
TEST_FILE1 = barrier_test.c

all:barrier.o $(TEST_FILE1)
	$(CC) -g -pthread $(TEST_FILE1) barrier.o -o barriertest

barrier.o:barrier.h barrier.c 
	gcc -c -pthread -Wall barrier.c
clean:
	rm -rf *.o barriertest *~