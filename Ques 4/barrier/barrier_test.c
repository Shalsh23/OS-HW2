#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "barrier.h"
#include "mythread.h"
#include "mythread_mutex.h"
#include "cv.h"

int v1=0, v2=0, v3=0;
mythread_barrier_t barrier;

void* f1()
{
	printf("\n in thread 1");
	sleep(1);
	v1 = 1;
	printf("\n v1 = %d, v2 = %d, v3 = %d :thread 1 \n now waiting for barrier \n", v1, v2, v3);
	
	//pthread_barrier_wait(&our_barrier);
	mythread_barrier_wait(&barrier);
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

void* f2()
{
	printf("\n in thread 2");
	sleep(2);
	v2 = 2;
	printf("\n v1 = %d, v2 = %d, v3 = %d :thread 2 \n now waiting for barrier \n", v1, v2, v3);
	
	//pthread_barrier_wait(&our_barrier);
	mythread_barrier_wait(&barrier);	
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

void* f3()
{
	printf("\n in thread 3");
	sleep(3);
	v3 = 3;
	printf("\n v1 = %d, v2 = %d, v3 = %d :thread 3 \n now waiting for barrier \n", v1, v2, v3);
	
	//pthread_barrier_wait(&our_barrier);	
	mythread_barrier_wait(&barrier);
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

int main()
{
	mythread_t t1, t2, t3;
	//pthread_barrier_init(&our_barrier, NULL, 3);
	//mythread_barrierattr_t* attr;
	mythread_barrier_init(&barrier, NULL, 3);

	int* res;

	res = (int*)mythread_create(&t1, NULL, &f1, NULL);
	res = (int*)mythread_create(&t2, NULL, &f2, NULL);
	res = (int*)mythread_create(&t3, NULL, &f3, NULL);

	mythread_join(t1, NULL);
	mythread_join(t2, NULL);
	mythread_join(t3, NULL);
	//pthread_barrier_destroy(&our_barrier);

	mythread_barrier_destroy(&barrier);

	printf("\n exiting \n");
}
