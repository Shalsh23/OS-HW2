#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "barrier.h"

int v1=0, v2=0, v3=0;

void* f1()
{
	printf("\n in thread 1");
	sleep(1);
	v1 = 1;
	
	//pthread_barrier_wait(&our_barrier);
	mythread_barrier_wait();
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

void* f2()
{
	printf("\n in thread 2");
	sleep(2);
	v2 = 2;
	
	//pthread_barrier_wait(&our_barrier);
	mythread_barrier_wait();	
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

void* f3()
{
	printf("\n in thread 3");
	sleep(3);
	v3 = 3;
	
	//pthread_barrier_wait(&our_barrier);	
	mythread_barrier_wait();
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

int main()
{
	pthread_t t1, t2, t3;
	//pthread_barrier_init(&our_barrier, NULL, 3);
	mythread_barrier_t barrier;
	mythread_barrierattr_t attr;
	mythread_barrier_init(barrier, attr, 3);

	int* res;

	res = (int*)pthread_create(&t1, NULL, &f1, NULL);
	res = (int*)pthread_create(&t2, NULL, &f2, NULL);
	res = (int*)pthread_create(&t3, NULL, &f3, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	//pthread_barrier_destroy(&our_barrier);

	mythread_barrier_destroy(barrier);

	printf("\n exiting \n");
}
