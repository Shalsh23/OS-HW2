#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int v1=0, v2=0, v3=0;
//pthread_barrier_t our_barrier;

int n; //barrier variable to keep the count of the number of threads
pthread_mutex_t my_lock;
pthread_cond_t my_cv;
int num_threads;

void mybarrier_init(int count)
{
	n = 0;
	num_threads = count;
	//my_lock = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&my_lock,NULL);
	//my_cv = PTHREAD_COND_INITIALIZER;
	pthread_cond_init(&my_cv,NULL);
	return;
}
int i = 0;
void mybarrier_wait()
{
	printf("in barrier wait %d", ++i);
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

	// {
	// 	if(n==0)
	// 		break;
	// }

	return;
}

void* f1()
{
	printf("\n in thread 1");
	sleep(1);
	v1 = 1;
	
	//pthread_barrier_wait(&our_barrier);
	mybarrier_wait();
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

void* f2()
{
	printf("\n in thread 2");
	sleep(2);
	v2 = 2;
	
	//pthread_barrier_wait(&our_barrier);
	mybarrier_wait();	
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

void* f3()
{
	printf("\n in thread 3");
	sleep(3);
	v3 = 3;
	
	//pthread_barrier_wait(&our_barrier);	
	mybarrier_wait();
	printf("\n v1 = %d, v2 = %d, v3 = %d", v1, v2, v3);
	return;
}

int main()
{
	pthread_t t1, t2, t3;
	//pthread_barrier_init(&our_barrier, NULL, 3);
	mybarrier_init(3);

	int* res;

	res = (int*)pthread_create(&t1, NULL, &f1, NULL);
	res = (int*)pthread_create(&t2, NULL, &f2, NULL);
	res = (int*)pthread_create(&t3, NULL, &f3, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	//pthread_barrier_destroy(&our_barrier);

	printf("\n exiting \n");
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
destro barrier
*/