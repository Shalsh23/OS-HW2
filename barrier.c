#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "barrier.h"
/*
int n; //barrier variable to keep the count of the number of threads
pthread_mutex_t my_lock;
pthread_cond_t my_cv;
int num_threads;
*/
void mythread_barrier_init(mythread_barrier_t barrier, mythread_barrierattr_t attr, unsigned count)
{
	n = 0;
	num_threads = count;
	pthread_mutex_init(&my_lock,NULL);
	pthread_cond_init(&my_cv,NULL);
	return;
}
int i = 0;
void mythread_barrier_wait()
{
	//printf("in barrier wait %d", ++i);
	pthread_mutex_lock(&my_lock);
		n++;
		if(n < num_threads)
		{
			while(n < num_threads)
			pthread_cond_wait(&my_cv, &my_lock);
		}
		else //if(n == num_threads)
		{
			pthread_cond_broadcast(&my_cv);
			//n = 0;	 //reinitializing the counter
		}
	pthread_mutex_unlock(&my_lock);

	return;
}

void mythread_barrier_destroy(mythread_barrier_t barrier)
{
	pthread_mutex_destroy(&my_lock);
	pthread_cond_destroy(&my_cv);
	n = NULL;
	num_threads = NULL;

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