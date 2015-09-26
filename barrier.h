#ifndef BARRIER_H
#define BARRIER_H

//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int n; //barrier variable to keep the count of the number of threads
pthread_mutex_t my_lock;
pthread_cond_t my_cv;
int num_threads;

typedef struct mythread_barrier 
{
} mythread_barrier_t;

typedef struct mythread_barrierattr 
{
} mythread_barrierattr_t;

extern void mythread_barrier_init(mythread_barrier_t barrier, mythread_barrierattr_t attr, unsigned count);
extern void mythread_barrier_wait();
extern void mythread_barrier_destroy(mythread_barrier_t barrier);

#endif
