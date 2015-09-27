#ifndef CV_H
#define CV_H

#include "mythread_mutex.h"
#include "mythread.h"
#include "myqueue.h"
#include <pthread.h>
typedef struct mythread_cond
{
    mythread_queue_t q;	                // Linked List
}* mythread_cond_t;
typedef struct mycondattr
{
	int value;
}*mycondattr_t;

extern int mythread_cond_init(mythread_cond_t *cond,const mycondattr_t attr);
extern int mythread_cond_destroy(mythread_cond_t *cond);
extern int mythread_cond_wait(mythread_cond_t *cond, mythread_mutex_t* mutex);
extern int mythread_cond_signal(mythread_cond_t *cond);
extern int mythread_cond_broadcast(mythread_cond_t *cond);
#endif 