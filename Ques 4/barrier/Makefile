CC=gcc
CFLAGS = -c -Wall
TEST_FILE1 = barrier_test.c

all:mymutex.o mycond.o barrier.o $(TEST_FILE1)
	$(CC) -g -pthread $(TEST_FILE1) mythread.a mythread_mutex.o cv.o barrier.o -o barriertest

mymutex.o:mythread_mutex.c mythread_mutex.h
	gcc -c -Wall mythread_mutex.c
mycond.o:cv.c cv.h
	gcc -c -Wall cv.c
barrier.o:barrier.h barrier.c 
	gcc -c -pthread -Wall barrier.c
clean:
	rm -rf *.o barriertest *~