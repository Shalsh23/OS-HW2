#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "barrier.h"
#include "mythread.h"
#include "mythread_mutex.h"
#include "cv.h"
/*
int n; //barrier variable to keep the count of the number of threads
pthread_mutex_t my_lock;
pthread_cond_t my_cv;
int num_threads;
*/
void mythread_barrier_init(mythread_barrier_t* pbarrier, mythread_barrierattr_t* pattr, unsigned count)
{
	pbarrier->n = 0;
	pbarrier->num_threads = count;
	mythread_mutex_init(&(pbarrier->my_lock),NULL);
	mythread_cond_init(&(pbarrier->my_cv),NULL);
	return;
}
//int i = 0;
void mythread_barrier_wait(mythread_barrier_t* pbarrier)
{
	mythread_mutex_lock(&(pbarrier->my_lock));
		pbarrier->n++;
		if(pbarrier->n < pbarrier->num_threads)
		{
			while(pbarrier->n < pbarrier->num_threads)
			mythread_cond_wait(&(pbarrier->my_cv), &(pbarrier->my_lock));
		}
		else //if(n == num_threads)
		{
			mythread_cond_broadcast(&(pbarrier->my_cv));
			//n = 0;	 //reinitializing the counter
		}
	mythread_mutex_unlock(&(pbarrier->my_lock));

	return;
}

void mythread_barrier_destroy(mythread_barrier_t* pbarrier)
{
	mythread_mutex_destroy(&(pbarrier->my_lock));
	mythread_cond_destroy(&(pbarrier->my_cv));
	pbarrier->n = NULL;
	pbarrier->num_threads = NULL;

	return;
}





/*
initialize barrier
create thread
	wait for barrier
		increment count
		wait on the cv	
		check if count==n?cv=1:do_nothing
			if (cv==1) braodcast
join thread
destroy barrier
*/