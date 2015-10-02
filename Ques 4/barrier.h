#ifndef BARRIER_H
#define BARRIER_H

//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"
#include "mythread_mutex.h"
#include "cv.h"



typedef struct mythread_barrier 
{
	int n; //barrier variable to keep the count of the number of threads
	mythread_mutex_t my_lock;
	mythread_cond_t my_cv;
	int num_threads;
} mythread_barrier_t;

typedef struct mythread_barrierattr 
{
} mythread_barrierattr_t;

extern void mythread_barrier_init(mythread_barrier_t* pbarrier, mythread_barrierattr_t* pattr, unsigned count);
extern void mythread_barrier_wait(mythread_barrier_t* pbarrier);
extern void mythread_barrier_destroy(mythread_barrier_t* pbarrier);

#endif
